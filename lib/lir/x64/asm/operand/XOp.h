#pragma once

#include <expected>

#include "XVReg.h"
#include "asm/x64/asm.h"


template<typename T>
concept XOpVariant = std::is_same_v<T, aasm::XmmReg> ||
    std::is_same_v<T, aasm::Address> ||
    std::is_integral_v<T>;

class XOp final {
public:
    template<XOpVariant T>
    XOp(const T& reg) noexcept:
        m_reg(reg) {}

    XOp(const XVReg& vreg) noexcept: m_reg(aasm::xmm0) {
        vreg.visit([&](const auto& reg) { m_reg = reg; });
    }

    [[nodiscard]]
    std::expected<aasm::Address, Error> as_address() const noexcept {
        if (const auto addr = std::get_if<aasm::Address>(&m_reg)) {
            return *addr;
        }

        return std::unexpected(Error::CastError);
    }

    friend std::ostream& operator<<(std::ostream& os, const XOp& reg) noexcept;

    template<typename T>
    void visit(const T& visitor) const {
        std::visit(visitor, m_reg);
    }

private:
    std::variant<aasm::XmmReg, aasm::Address, std::int64_t> m_reg;
};