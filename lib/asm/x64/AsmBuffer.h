#pragma once

#include <cstdint>
#include <vector>

#include "Assembler.h"

namespace aasm {
    /**
     * Represents a buffer of x64 assembly instructions with metadata for label and call resolution.
     */
    class AsmBuffer final {
    public:
        constexpr AsmBuffer(std::vector<std::uint32_t>&& label_table, std::vector<X64Instruction>&& instructions) noexcept:
            m_label_table(std::move(label_table)),
            m_instructions(std::move(instructions)) {}

        /**
         * Emits the opcodes into the provided code buffer.
         * @param buffer The code buffer to emit the instructions into.
         */
        template<CodeBuffer Buffer>
        constexpr std::vector<Relocation> emit(Buffer& buffer) const {
            return details::Assembler::assemble(buffer, m_instructions, m_label_table);
        }

        /**
         * Returns number of instructions.
         */
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