#include "JitModule.h"

#include "OpCodeBuffer.h"
#include "asm/x64/SizeEvaluator.h"
#include "utility/ArithmeticUtils.h"

static constexpr auto PAGE_SIZE = 4096;

struct MmapAllocation final {
    std::span<std::uint8_t> memory;
    std::span<std::uint8_t> plt_table;
    std::span<std::uint8_t> code_buffer;
};

static MmapAllocation map_memory(const std::size_t plt_size, const std::size_t code_buffer_size) {
    const auto plt_table_size = align_up(plt_size, PAGE_SIZE);
    const auto total_size = plt_table_size + code_buffer_size;
    const auto memory = static_cast<std::uint8_t*>(mmap(nullptr, total_size,
                                              PROT_READ | PROT_WRITE | PROT_EXEC,
                                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    return {std::span(memory, total_size), std::span(memory, plt_table_size), std::span(memory + plt_table_size, code_buffer_size)};
}

class RelocResolver final {
public:
    explicit RelocResolver(const std::unordered_map<const aasm::Symbol*, std::size_t>& plt_table,
        const AsmModule& module,
        const std::span<std::uint8_t> code_buffer, const std::size_t code_buffer_offset) noexcept:
        m_plt_table(plt_table),
        m_module(module),
        jit_assembler(code_buffer),
        m_code_buffer_offset(code_buffer_offset) {}

    void run() {
        assemble_global_slots();
        assemble_functions();
        try_resolve_relocations();
    }

    std::unordered_map<const aasm::Symbol*, JitDataChunk> result() {
        return std::move(offset_table);
    }

private:
    void assemble_functions() {
        for (const auto& [name, emitter] : m_module.m_asm_buffers) {
            assemble_slot(name, emitter);
        }
    }

    void assemble_global_slots() {
        for (const auto& [name, slot]: m_module.m_global_slots) {
            assemble_slot(name, slot);
        }
    }

    template<typename T>
    void assemble_slot(const aasm::Symbol* name, T& element) {
        const auto start = jit_assembler.size();
        auto reloc = element.emit(jit_assembler);
        relocation_table.emplace(name, std::move(reloc));
        offset_table.emplace(name, JitDataChunk(start, jit_assembler.size() - start));
    }

    void try_resolve_relocations() {
        for (const auto& relocation : relocation_table | std::views::values) {
            for (const auto& reloc : relocation) {
                switch (reloc.type()) {
                    case RelType::X86_64_NONE:     break;
                    case RelType::X86_64_PC32:     try_patch_relocation(reloc); break;
                    case RelType::X86_64_PLT32:    try_plt_patch_relocation(reloc); break;
                    case RelType::X86_64_GLOB_DAT: try_glob_patch_relocation(reloc); break;
                    default: die("Unsupported relocation type: {}", static_cast<std::uint8_t>(reloc.type()));
                }
            }
        }
    }

    void try_glob_patch_relocation(const aasm::Relocation& reloc) {
        const auto chunk = offset_table.find(reloc.symbol());
        if (chunk == offset_table.end()) {
            die("Relocation for label '{}' not found in offset table", reloc.symbol_name());
        }

        const auto& [start, _] = chunk->second;
        const auto offset = static_cast<std::int64_t>(start) - reloc.offset() + reloc.displacement();
        if (!std::in_range<std::int32_t>(offset)) {
            die("Offset {} is out of range for 32-bit patching", offset);
        }

        const auto address = reinterpret_cast<std::int64_t>(jit_assembler.data()) + offset+8;
        jit_assembler.patch64(static_cast<std::int64_t>(reloc.offset()) - sizeof(std::int64_t), address);
    }

    void try_plt_patch_relocation(const aasm::Relocation& reloc) {
        const auto& external = m_plt_table.find(reloc.symbol());
        if (external == m_plt_table.end()) {
            die("PLT relocation for label '{}' not found in external symbols", reloc.symbol_name());
        }
        const auto offset = static_cast<std::int64_t>(external->second) - (reloc.offset() + static_cast<std::int64_t>(m_code_buffer_offset)) + reloc.displacement();
        if (!std::in_range<std::int32_t>(offset)) {
            die("Offset {} is out of range for 32-bit PLT patching", offset);
        }
        jit_assembler.patch32(static_cast<std::int64_t>(reloc.offset()) - sizeof(std::int32_t), offset);
    }

    void try_patch_relocation(const aasm::Relocation& reloc) {
        const auto chunk = offset_table.find(reloc.symbol());
        if (chunk == offset_table.end()) {
            die("Relocation for label '{}' not found in offset table", reloc.symbol_name());
        }

        const auto& [start, _] = chunk->second;
        const auto offset = static_cast<std::int64_t>(start) - reloc.offset() + reloc.displacement();
        if (!std::in_range<std::int32_t>(offset)) {
            die("Offset {} is out of range for 32-bit patching", offset);
        }

        jit_assembler.patch32(static_cast<std::int64_t>(reloc.offset()) - sizeof(std::int32_t), offset);
    }

    const std::unordered_map<const aasm::Symbol*, std::size_t>& m_plt_table;
    const AsmModule& m_module;
    OpCodeBuffer jit_assembler;
    std::size_t m_code_buffer_offset;

    std::unordered_map<const aasm::Symbol*, std::vector<aasm::Relocation>> relocation_table;
    std::unordered_map<const aasm::Symbol*, JitDataChunk> offset_table;
};

JitModule JitModule::assembly(const std::unordered_map<const aasm::Symbol *, std::size_t> &external_symbols, AsmModule &&module) {
    const auto code_buffer_size = aasm::ModuleSizeEvaluator::module_size_eval(module);
    const auto plt_size = external_symbols.size() * sizeof(std::int64_t);
    const auto memory = map_memory(plt_size, code_buffer_size);

    std::unordered_map<const aasm::Symbol*, std::size_t> plt_table;
    std::span plt_table_span{reinterpret_cast<std::uint64_t*>(memory.plt_table.data()), memory.plt_table.size() / sizeof(std::uint64_t)};

    std::size_t plt_table_offset{};
    for (const auto& [symbol, address] : external_symbols) {
        plt_table_span[plt_table_offset] = address;
        plt_table_offset += 1;
        plt_table.emplace(symbol, sizeof(std::int64_t) * (plt_table_offset-1));
    }

    RelocResolver resolver(plt_table, module, memory.code_buffer, memory.plt_table.size());
    resolver.run();

    JitDataBlob code_blob(resolver.result(), memory.code_buffer);
    return {std::move(module.m_symbol_table), memory.memory, std::move(code_blob)};
}

std::ostream & operator<<(std::ostream &os, const JitModule &blob) {
    return os << blob.m_code_blob;
}