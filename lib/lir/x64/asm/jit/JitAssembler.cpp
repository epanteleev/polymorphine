#include <sys/mman.h>

#include "JitAssembler.h"
#include "asm/SizeEvaluator.h"

class RelocResolver final {
public:
    explicit RelocResolver(ObjModule&& module) noexcept:
        m_module(std::move(module)),
        m_buffer_size(module_size_eval(m_module)),
        m_mapped_buffer(map_memory(m_buffer_size)),
        jit_assembler(m_mapped_buffer) {}

    JitCodeBlob run() {
        resolve_and_patch_labels();
        try_resolve_relocations();
        return {m_module.symbol_table(), std::move(offset_table), m_mapped_buffer, m_buffer_size};
    }

private:
    void resolve_and_patch_labels() {
        for (const auto& [name, emitter] : m_module.assembler()) {
            const auto start = jit_assembler.size();
            auto reloc = emitter.emit(jit_assembler);
            relocation_table.emplace(name, std::move(reloc));

            offset_table.emplace(name, JitCodeChunk(start, jit_assembler.size() - start));
        }
    }

    void try_resolve_relocations() {
        for (const auto& relocation : relocation_table | std::views::values) {
            for (const auto& reloc : relocation) {
                switch (reloc.type()) {
                    case RelType::R_X86_64_NONE: break;
                    case RelType::R_X86_64_PC32: try_patch_relocation(reloc); break;
                    default: die("Unsupported relocation type: {}", static_cast<std::uint8_t>(reloc.type()));
                }
            }
        }
    }

    void try_patch_relocation(const aasm::Relocation& reloc) {
        const auto chunk = offset_table.find(reloc.symbol());
        if (chunk == offset_table.end()) {
            die("Relocation for label '{}' not found in offset table", reloc.symbol_name());
        }

        const auto& [start, size] = chunk->second;
        const auto offset = static_cast<std::int64_t>(start) - reloc.offset();
        if (!std::in_range<std::int32_t>(offset)) {
            die("Offset {} is out of range for 32-bit patching", offset);
        }

        jit_assembler.patch32(reloc.offset() - sizeof(std::int32_t), offset);
    }

    static std::uint8_t* map_memory(const std::size_t buffer_size) {
        return static_cast<std::uint8_t*>(mmap(nullptr, buffer_size,
                                                  PROT_READ | PROT_WRITE | PROT_EXEC,
                                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    }

    static std::size_t module_size_eval(const ObjModule& masm) {
        std::size_t acc = 0;
        for (const auto& emitter : masm.assembler() | std::views::values) {
            acc += aasm::SizeEvaluator::emit(emitter);
        }

        return acc;
    }

    ObjModule m_module;
    std::size_t m_buffer_size;
    std::uint8_t* m_mapped_buffer;
    JitAssembler jit_assembler;

    std::unordered_map<const aasm::Symbol*, std::vector<aasm::Relocation>> relocation_table;
    std::unordered_map<const aasm::Symbol*, JitCodeChunk> offset_table;
};


JitCodeBlob JitAssembler::assembly(ObjModule&& module) {
    RelocResolver resolver(std::move(module));
    return resolver.run();
}