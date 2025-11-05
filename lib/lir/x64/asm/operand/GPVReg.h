#pragma once

#include <variant>
#include <expected>

#include "Constrains.h"

class GPVReg final {
public:
    template<GPVRegVariant T>
    GPVReg(const T& reg) noexcept:
        m_reg(reg) {}

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

    [[nodiscard]]
    std::expected<aasm::Address, Error> as_address() const noexcept {
        if (const auto addr = std::get_if<aasm::Address>(&m_reg)) {
            return *addr;
        }

        return std::unexpected(Error::CastError);
    }

    bool operator==(const GPVReg &other) const noexcept {
        if (this == &other) {
            return true;
        }

        return m_reg == other.m_reg;
    }

    friend std::ostream& operator<<(std::ostream& os, const GPVReg& reg) noexcept;

private:
    std::variant<aasm::GPReg, aasm::Address> m_reg;
};