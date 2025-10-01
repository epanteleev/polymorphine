#pragma once
#include <expected>

#include "asm/x64/asm.h"
#include "utility/Error.h"


template<typename T>
concept XVRegVariant = std::is_same_v<T, aasm::XmmReg> ||
    std::is_same_v<T, aasm::Address>;

class XVReg final {
public:
    template<XVRegVariant T>
    XVReg(const T& reg) noexcept:
        m_reg(reg) {}

    template<typename T>
    void visit(const T& visitor) const {
        std::visit(visitor, m_reg);
    }

    [[nodiscard]]
    std::expected<aasm::XmmReg, Error> as_xmm_reg() const noexcept {
        if (const auto reg = std::get_if<aasm::XmmReg>(&m_reg)) {
            return *reg;
        }

        return std::unexpected(Error::CastError);
    }

    [[nodiscard]]
    std::expected<aasm::Address, Error> as_address() const noexcept {
        if (const auto addr = std::get_if<aasm::Address>(&m_reg)) {
            return *addr;
        }

        return std::unexpected(Error::CastError);
    }

    bool operator==(const XVReg &other) const noexcept {
        if (this == &other) {
            return true;
        }

        return m_reg == other.m_reg;
    }

    friend std::ostream& operator<<(std::ostream& os, const XVReg& reg) noexcept;

private:
    std::variant<aasm::XmmReg, aasm::Address> m_reg;
};