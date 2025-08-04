#include <sys/mman.h>

#include "JitAssembler.h"
#include "asm/SizeEvaluator.h"
#include "utility/Align.h"

static constexpr auto PAGE_SIZE = 4096;
static std::pair<std::uint8_t*, std::uint8_t*> map_memory(const std::size_t plt_size, const std::size_t code_buffer_size) {

    const auto memory = static_cast<std::uint8_t*>(mmap(nullptr, plt_size + code_buffer_size,
                                              PROT_READ | PROT_WRITE | PROT_EXEC,
                                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    return {memory, memory + plt_size};
}

static std::size_t module_size_eval(const ObjModule& masm) {
    std::size_t acc = 0;
    for (const auto& emitter : masm.assembler() | std::views::values) {
        acc += aasm::SizeEvaluator::emit(emitter);
    }

    return acc;
}


class RelocResolver final {
public:
    explicit RelocResolver(const std::unordered_map<const aasm::Symbol*, std::size_t>& plt_table,
        const ObjModule& module,
        const std::size_t buffer_size,
        std::uint8_t* buffer) noexcept:
        m_plt_table(plt_table),
        m_module(module),
        m_buffer_size(buffer_size),
        m_mapped_code_buffer(buffer),
        jit_assembler(buffer) {}

    JitCodeBlob run() {
        resolve_and_patch_labels();
        try_resolve_relocations();
        return {m_module.symbol_table(), std::move(offset_table), m_mapped_code_buffer, m_buffer_size};
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
                    case RelType::R_X86_64_PLT32: try_plt_patch_relocation(reloc); break;
                    default: die("Unsupported relocation type: {}", static_cast<std::uint8_t>(reloc.type()));
                }
            }
        }
    }

    void try_plt_patch_relocation(const aasm::Relocation& reloc) {
        const auto& external = m_plt_table.find(reloc.symbol());
        if (external == m_plt_table.end()) {
            die("PLT relocation for label '{}' not found in external symbols", reloc.symbol_name());
        }
        const auto offset = static_cast<std::int64_t>(external->second) - (reloc.offset() + 8);
        if (!std::in_range<std::int32_t>(offset)) {
            die("Offset {} is out of range for 32-bit PLT patching", offset);
        }
        jit_assembler.patch32(static_cast<std::int64_t>(reloc.offset()), offset);
    }

    void try_patch_relocation(const aasm::Relocation& reloc) {
        const auto chunk = offset_table.find(reloc.symbol());
        if (chunk == offset_table.end()) {
            die("Relocation for label '{}' not found in offset table", reloc.symbol_name());
        }

        const auto& [start, _] = chunk->second;
        const auto offset = static_cast<std::int64_t>(start) - reloc.offset();
        if (!std::in_range<std::int32_t>(offset)) {
            die("Offset {} is out of range for 32-bit patching", offset);
        }

        jit_assembler.patch32(static_cast<std::int64_t>(reloc.offset()) - sizeof(std::int32_t), offset);
    }

    const std::unordered_map<const aasm::Symbol*, std::size_t>& m_plt_table;
    const ObjModule& m_module;
    std::size_t m_buffer_size;
    std::uint8_t* m_mapped_code_buffer;
    JitAssembler jit_assembler;

    std::unordered_map<const aasm::Symbol*, std::vector<aasm::Relocation>> relocation_table;
    std::unordered_map<const aasm::Symbol*, JitCodeChunk> offset_table;
};


JitCodeBlob JitAssembler::assembly(const std::unordered_map<const aasm::Symbol*, std::size_t>& external_symbols, const ObjModule& module) {
    const auto code_buffer_size = module_size_eval(module);
    const auto plt_size = external_symbols.size() * sizeof(std::int64_t);
    const auto [plt_table_start, mapped_code_buffer] = map_memory(plt_size, code_buffer_size);

    std::unordered_map<const aasm::Symbol*, std::size_t> plt_table;
    std::span plt_table_span(reinterpret_cast<std::uint64_t*>(plt_table_start), plt_size / sizeof(std::uint64_t));

    std::size_t plt_table_offset{};
    for (const auto& [symbol, address] : external_symbols) {
        plt_table_span[plt_table_offset] = address;
        plt_table_offset += 1;
        plt_table.emplace(symbol, sizeof(std::int64_t) * (plt_table_offset-1));
    }

    RelocResolver resolver(plt_table, module, code_buffer_size, mapped_code_buffer);
    return resolver.run();
}