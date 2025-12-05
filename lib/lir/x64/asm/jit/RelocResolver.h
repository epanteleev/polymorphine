#pragma once

namespace details {
    class RelocResolver final {
    public:
        explicit RelocResolver(const std::unordered_map<const aasm::Symbol*, std::size_t>& plt_table,
            const aasm::AsmModule& module,
            const std::span<std::uint8_t> code_buffer, const std::size_t code_buffer_offset) noexcept:
            m_plt_table(plt_table),
            m_module(module),
            jit_assembler(code_buffer),
            m_code_buffer_offset(code_buffer_offset) {}

        void run() {
            relocation_table.resize(m_module.m_asm_buffers.size() + m_module.m_global_slots.size());

            for (const auto& [name, slot]: m_module.m_global_slots) {
                assemble_slot(name, slot);
            }

            for (const auto& [name, emitter] : m_module.m_asm_buffers) {
                assemble_slot(name, emitter);
            }

            try_resolve_relocations();
        }

        std::unordered_map<const aasm::Symbol*, JitDataChunk> result() {
            return std::move(offset_table);
        }

    private:
        template<typename T>
        void assemble_slot(const aasm::Symbol* name, T& element) {
            const auto start = jit_assembler.size();
            auto reloc = element.emit(jit_assembler);

            relocation_table.push_back(std::move(reloc));
            [[maybe_unused]]
            const auto [_unused2, has2] = offset_table.emplace(name, JitDataChunk(start, jit_assembler.size() - start));
            assertion(has2, "Offset for symbol already exists: {}", name->name());
        }

        void try_resolve_relocations() {
            for (const auto& relocation : relocation_table) {
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
            const auto address = reinterpret_cast<std::int64_t>(jit_assembler.data()) + reloc.displacement();
            jit_assembler.patch64(reloc.offset(), address);
        }

        void try_plt_patch_relocation(const aasm::Relocation& reloc) {
            const auto& external = m_plt_table.find(reloc.symbol());
            if (external == m_plt_table.end()) {
                die("PLT relocation for symbol '{}' not found in external symbols", reloc.symbol_name());
            }

            const auto offset = static_cast<std::int64_t>(external->second) - static_cast<std::int64_t>(m_code_buffer_offset) - reloc.displacement();
            jit_assembler.patch32(reloc.offset(), checked_cast<std::int32_t>(offset));
        }

        void try_patch_relocation(const aasm::Relocation& reloc) {
            const auto chunk = offset_table.find(reloc.symbol());
            if (chunk == offset_table.end()) {
                die("Relocation for label '{}' not found in offset table", reloc.symbol_name());
            }

            const auto& [start, _] = chunk->second;
            const auto offset = static_cast<std::int64_t>(start) - reloc.displacement();
            jit_assembler.patch32(reloc.offset(), checked_cast<std::int32_t>(offset));
        }

        const std::unordered_map<const aasm::Symbol*, std::size_t>& m_plt_table;
        const aasm::AsmModule& m_module;
        OpCodeBuffer jit_assembler;
        std::size_t m_code_buffer_offset;

        std::vector<std::vector<aasm::Relocation>> relocation_table;
        std::unordered_map<const aasm::Symbol*, JitDataChunk> offset_table;
    };
}
