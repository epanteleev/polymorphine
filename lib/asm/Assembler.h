#pragma once

#include <deque>

#include "encoding.h"
#include "Register.h"
#include <iosfwd>
#include <span>

#include "Address.h"


class Assembler final {
public:
    /*
     * REX prefix contains bits [0, 1, 0, 0, W, R, X, B]
     * W: 1 if operands are 64 bit.
     * R: Extension of ModRM reg field (most significant bit)
     * X: Extension of SIB index field
     * B: Extension of ModRM r/m field, SIB base field, or Opcode reg field
     */
    static constexpr std::uint8_t REX = 0x40;
    static constexpr std::uint8_t REX_W = 0x48; // REX prefix for 64-bit operands

    void ret();

    void popq(GPReg r);
    void popw(GPReg r);

    void popq(const Address& addr);
    void popw(const Address& addr);

    void pushq(GPReg r);
    void pushw(GPReg r);

    void pushq(const Address& addr);
    void pushw(const Address& addr);

    void movq(GPReg src, GPReg dest);

    void print_codes(std::ostream &os) const;

    [[nodiscard]]
    std::size_t to_byte_buffer(std::span<std::uint8_t> buffer) const;
private:
    std::deque<code> m_inst;
};
