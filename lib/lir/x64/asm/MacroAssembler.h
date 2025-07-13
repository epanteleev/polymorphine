#pragma once
#include "asm/Address.h"
#include "asm/Assembler.h"


class MacroAssembler final {
public:
    void ret() {
        m_asm.ret();
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void pop(const std::size_t size, const Op& r) {
        m_asm.pop(size, r);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void push(const std::size_t size, const Op& r) {
        m_asm.push(size, r);
    }

    void copy(const std::size_t size, const aasm::GPReg src, const aasm::GPReg dst) {
        if (src == dst) {
            return;
        }

        m_asm.mov(size, src, dst);
    }

    void copy(const std::size_t size, const std::int64_t src, const aasm::GPReg dst) {
        m_asm.mov(size, src, dst);
    }

    friend std::ostream& operator<<(std::ostream& os, const MacroAssembler& masm);

    template<typename  Buffer>
    void emit(Buffer& buffer) const {
        m_asm.emit(buffer);
    }

private:
    aasm::Assembler m_asm{};
};

inline std::ostream & operator<<(std::ostream &os, const MacroAssembler& masm) {
    return os << masm.m_asm;
}
