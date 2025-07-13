#pragma once
#include <variant>
#include <expected>

#include "asm/Address.h"
#include "asm/Register.h"


class GPVReg final {
public:
    GPVReg(const aasm::GPReg reg) noexcept
        : m_reg(reg) {}

    GPVReg(const aasm::Address addr) noexcept:
        m_reg(addr) {}

    friend std::ostream& operator<<(std::ostream& os, const GPVReg& reg) noexcept;

    template<typename T>
    void visit(const T& visitor) const {
        std::visit(visitor, m_reg);
    }

    [[nodiscard]]
    std::expected<aasm::GPReg, Error> as_gp_reg() const noexcept {
        if (const auto reg = std::get_if<aasm::GPReg>(&m_reg)) {
            return *reg;
        }

        return std::unexpected(Error::CastError);
    }

private:
    std::variant<aasm::GPReg, aasm::Address> m_reg;
};

inline std::ostream & operator<<(std::ostream &os, const GPVReg &reg) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, aasm::GPReg>) {
            os << val.name(8);
        } else if constexpr (std::is_same_v<T, aasm::Address>) {
            os << val;
        } else {
            static_assert(false, "Unsupported type in GPVReg variant");
        }
    };

    std::visit(visitor, reg.m_reg);
    return os;
}
