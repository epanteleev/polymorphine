#pragma once

#include <vector>
#include <ranges>
#include <unordered_map>

#include "instruction/CPUInstruction.h"

namespace aasm::details {
    class Assembler final {
    public:
        /**
         * Assembles a vector of X64 instructions into a code buffer.
         * @param buffer The code buffer to emit the instructions into.
         * @param instructions The vector of X64 instructions to assemble.
         * @param label_table A vector mapping labels to instruction indices.
         */
        template <CodeBuffer Buffer>
        static constexpr std::vector<Relocation> assemble(Buffer& buffer, const std::vector<X64Instruction> &instructions, const std::vector<std::uint32_t> &label_table) {
            Assembler assembler(instructions, label_table);
            return assembler.emit(buffer);
        }

    private:
        constexpr explicit Assembler(const std::vector<X64Instruction> &instructions, const std::vector<std::uint32_t> &label_table) noexcept:
            m_label_table(label_table),
            m_instructions(instructions) {}

        template<CodeBuffer Buffer>
        constexpr std::vector<Relocation> emit(Buffer &buffer) {
            offsets_from_start.reserve(m_instructions.size());
            unresolved_labels.resize(m_instructions.size());

            for (const auto &instruction: m_instructions) {
                offsets_from_start.push_back(buffer.size());
                instruction.visit([&](const auto &var) { emit_instruction(buffer, var); });
            }

            resolve_and_patch(buffer);
            return std::move(m_relocations);
        }

        template<CodeBuffer Buffer, typename T>
        constexpr void emit_instruction(Buffer& buffer, const T& inst) {
            if constexpr (std::is_same_v<T, Jmp> || std::is_same_v<T, Jcc>) {
                emit_jump(buffer, inst);

            } else if constexpr (MemoryInstruction<T>) {
                if (const auto reloc = inst.emit(buffer); reloc.has_value()) {
                    m_relocations.emplace_back(std::move(reloc.value()));
                }

            } else {
                inst.emit(buffer);
            }
        }

        template<CodeBuffer Buffer, typename T>
        constexpr void emit_jump(Buffer& buffer, const T& var) {
            const auto inst_idx = m_label_table[var.label().id()];
            if (inst_idx == constants::NO_OFFSET) {
                die("Label defined, but not set");
            }

            if (inst_idx >= offsets_from_start.size()) {
                // Label is defined, but not set yet. Handle it as unresolved.
                var.emit_unresolved32(buffer);
                unresolved_labels[var.label().id()].emplace_back(buffer.size());
            } else {
                const auto offset_from_function_start = offsets_from_start[inst_idx];
                var.emit(buffer, offset_from_function_start - static_cast<std::int64_t>(buffer.size()));
            }
        }

        template<CodeBuffer Buffer>
        constexpr void resolve_and_patch(Buffer &buffer) {
            for (const auto label_id: std::views::iota(0U, m_label_table.size())) {
                const auto label_offset = offsets_from_start[m_label_table[label_id]];
                for (const auto gap: unresolved_labels[label_id]) {
                    buffer.patch32(gap - sizeof(std::int32_t), label_offset - gap);
                }
            }
        }

        const std::vector<std::uint32_t>& m_label_table; // HashMap from 'label' to instruction index
        const std::vector<X64Instruction> &m_instructions;

        std::vector<std::int32_t> offsets_from_start; // instruction index to offset from code block start
        std::vector<std::vector<std::int32_t>> unresolved_labels; // Hashmap from 'label' to vector of offsets where jmp operand must be patched.

        std::vector<Relocation> m_relocations;
    };
}
