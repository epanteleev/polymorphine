#pragma once

#include <cstdint>
#include <vector>
#include <ranges>

#include "Register.h"
#include "Address.h"
#include "AsmBuffer.h"
#include "instruction/CPUInstruction.h"

namespace aasm {
    class AsmBuffer final {
    public:
        constexpr AsmBuffer(std::vector<std::uint32_t>&& label_table, std::vector<X64Instruction>&& instructions) noexcept:
            m_label_table(std::move(label_table)),
            m_instructions(std::move(instructions)) {}

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            // instruction index to offset from function start
            std::vector<std::int32_t> offsets_from_start;
            offsets_from_start.reserve(m_instructions.size());

            // Hashmap from 'label' to vector of offsets where jmp operand must be patched.
            std::vector<std::vector<std::int32_t>> unresolved_labels;
            unresolved_labels.resize(m_instructions.size());

            const auto visitor = [&]<typename T>(const T &var) {
                if constexpr (std::is_same_v<T, Jmp> || std::is_same_v<T, Jcc>) {
                    const auto inst_idx = m_label_table[var.label().id()];
                    if (inst_idx == constants::NO_OFFSET) {
                        die("Label defined, but not set");
                    }

                    if (inst_idx >= offsets_from_start.size()) {
                        var.emit_unresolved32(buffer);
                        unresolved_labels[var.label().id()].emplace_back(buffer.size());
                    } else {
                        const auto offset_from_function_start = offsets_from_start[inst_idx];
                        var.emit(buffer, offset_from_function_start - static_cast<std::int64_t>(buffer.size()));
                    }
                } else {
                    var.emit(buffer);
                }
            };

            for (const auto& instruction: m_instructions) {
                offsets_from_start.push_back(buffer.size());
                instruction.visit(visitor);
            }

            for (const auto label_id: std::views::iota(0U, m_label_table.size())) {
                const auto label_offset = offsets_from_start[m_label_table[label_id]];
                for (auto gaps: unresolved_labels[label_id]) {
                    buffer.patch32(gaps - sizeof(std::int32_t), label_offset - gaps);
                }
            }
        }

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return m_instructions.size();
        }

        friend std::ostream &operator<<(std::ostream &os, const AsmBuffer &asm_buff);

    private:
        std::vector<std::uint32_t> m_label_table; // HashMap from 'label' to instruction index
        std::vector<X64Instruction> m_instructions;
    };

    std::ostream & operator<<(std::ostream &os, const AsmBuffer &asm_buff);
}