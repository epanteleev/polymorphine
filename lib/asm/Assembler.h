#pragma once

#include <deque>
#include <iosfwd>
#include <span>

#include "AbstractAssembler.h"
#include "Register.h"
#include "Address.h"

namespace aasm {
    struct code final {
        code() = default;
        explicit code(const std::uint8_t c): len(1) {
            val[0] = c;
        }

        void emit8(const std::uint8_t c) noexcept {
            val[len++] = c;
        }

        std::uint8_t& last() {
            return val[len - 1];
        }

        unsigned char val[15]{};
        std::uint8_t len{};
    };

    class Assembler final: public AbstractAssembler {
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

        void ret() override;

        void popq(GPReg r) override;
        void popw(GPReg r) override;

        void popq(const Address& addr) override;
        void popw(const Address& addr) override;

        void pushq(GPReg r) override;
        void pushw(GPReg r) override;

        void pushq(const Address& addr) override;
        void pushw(const Address& addr) override;

        void movq(GPReg src, GPReg dest) override;
        void movl(GPReg src, GPReg dest) override;
        void movw(GPReg src, GPReg dest) override;
        void movb(GPReg src, GPReg dest) override;

        void print_codes(std::ostream &os) const;

        [[nodiscard]]
        std::size_t to_byte_buffer(std::span<std::uint8_t> buffer) const;
    private:
        std::deque<code> m_inst;
    };

    template<typename T>
    static constexpr bool in_byte_range(T arg) {
        return arg >= -128 && arg <= 127;
    }
}