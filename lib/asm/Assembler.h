#pragma once

#include <deque>
#include <iosfwd>

#include "Register.h"
#include "Address.h"
#include "CPUInstruction.h"

namespace aasm {
    class Assembler final {
    public:
        static constexpr auto PREFIX_OPERAND_SIZE = 0x66;

        /*
         * REX prefix contains bits [0, 1, 0, 0, W, R, X, B]
         * W: 1 if operands are 64 bit.
         * R: Extension of ModRM reg field (most significant bit)
         * X: Extension of SIB index field
         * B: Extension of ModRM r/m field, SIB base field, or Opcode reg field
         */
        static constexpr std::uint8_t REX = 0x40;
        static constexpr std::uint8_t REX_W = 0x48; // REX prefix for 64-bit operands

        void ret() {
            m_instructions.emplace_back(Ret());
        }

        void pop(const std::size_t size, const GPReg r) {
            m_instructions.emplace_back(PopR(size, r));
        }

        void pop(const std::size_t size, const Address& addr) {
            m_instructions.emplace_back(PopM(size, addr));
        }

        void push(const std::size_t size, const GPReg r) {
            m_instructions.emplace_back(PushR(size, r));
        }

        void push(const std::size_t size, const Address& addr) {
            m_instructions.emplace_back(PushM(size, addr));
        }

        void push(const std::size_t size, const std::int32_t value) {
            assertion(in_size_range(value, size), "Value {} is not in range for size {}", value, size);
            m_instructions.emplace_back(PushI(value, size));
        }

        void mov(const std::size_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(MovRR(size, src, dst));
        }

        void mov(const std::size_t size, const std::int64_t src, const GPReg dst) {
            m_instructions.emplace_back(MovRI(size, src, dst));
        }

        void print_mnemonics(std::ostream &os) const {
            for (auto& instruction: m_instructions) {
                os << instruction << std::endl;
            }
        }

        template<CodeBuffer Buffer>
        void emit(Buffer& buffer) const {
            for (const auto& instruction: m_instructions) {
                instruction.emit(buffer);
            }
        }

    private:
        std::deque<X64Instruction> m_instructions;
    };
}
