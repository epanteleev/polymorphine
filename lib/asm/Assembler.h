#pragma once

#include <deque>
#include <iosfwd>
#include <span>

#include "Register.h"
#include "Address.h"
#include "CPUInstruction.h"

namespace aasm {
    struct code final {
        code() = default;
        explicit code(const std::uint8_t c): len(1) {
            val[0] = c;
        }

        void emit8(const std::uint8_t c) noexcept {
            val[len++] = c;
        }

        void emit32(const std::uint32_t c) noexcept {
            val[len++] = (c >> 24) & 0xFF;
            val[len++] = (c >> 16) & 0xFF;
            val[len++] = (c >> 8) & 0xFF;
            val[len++] = c & 0xFF;
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return len;
        }

        unsigned char val[15]{};
        std::uint8_t len{};
    };

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

        void pop(std::size_t size, const GPReg r) {
            m_instructions.emplace_back(PopR(size, r));
        }

        void pop(std::size_t size, const Address& addr) {
            m_instructions.emplace_back(PopM(size, addr));
        }

        void push(std::size_t size, const GPReg r) {
            m_instructions.emplace_back(PushR(size, r));
        }

        void push(std::size_t size, const Address& addr) {
            m_instructions.emplace_back(PushM(size, addr));
        }

        void mov(std::size_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(MovRR(size, src, dst));
        }

        void print_codes(std::ostream &os) const {
            for (auto& instruction: m_instructions) {
                os << instruction << std::endl;
            }
        }

        [[nodiscard]]
        std::size_t to_byte_buffer(std::span<std::uint8_t> buffer) const;
    private:
        std::deque<X64Instruction> m_instructions;
        //std::deque<code> m_inst;
    };
}
