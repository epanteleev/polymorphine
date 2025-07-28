#pragma once

#include "GPVReg.h"
#include "asm/asm.h"


class GPOp final {
public:
    GPOp(const aasm::GPReg reg) noexcept
        : m_reg(reg) {}

    GPOp(const aasm::Address addr) noexcept:
        m_reg(addr) {}

    GPOp(const std::int64_t imm) noexcept:
        m_reg(imm) {}

    GPOp(const GPVReg& vreg) noexcept: m_reg(aasm::rax) {
        vreg.visit([&](const auto& reg) { m_reg = reg; });
    }

    friend std::ostream& operator<<(std::ostream& os, const GPOp& reg) noexcept;

    template<typename T>
    void visit(const T& visitor) const {
        std::visit(visitor, m_reg);
    }

private:
    std::variant<aasm::GPReg, aasm::Address, std::int64_t> m_reg;
};

inline std::ostream & operator<<(std::ostream &os, const GPOp &reg) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, aasm::GPReg>) {
            os << val.name(8);
        } else if constexpr (std::is_same_v<T, aasm::Address>) {
            os << val;
        } else if constexpr (std::is_integral_v<T>) {
            os << "0x" << std::hex << val;
        } else {
            static_assert(false, "Unsupported type in GPOp variant");
        }
    };

    std::visit(visitor, reg.m_reg);
    return os;
}