#pragma once

#include "asm/x64/asm.h"

class TemporalRegs final {
public:
    explicit TemporalRegs() noexcept = default;
    explicit TemporalRegs(const std::optional<aasm::GPReg>& gp_temp1) noexcept:
        m_gp_temp1(gp_temp1) {}

    explicit TemporalRegs(const std::optional<aasm::GPReg>& gp_temp1, const std::optional<aasm::GPReg>& gp_temp2) noexcept:
        m_gp_temp1(gp_temp1),
        m_gp_temp2(gp_temp2) {}

    [[nodiscard]]
    aasm::GPReg gp_temp1() const noexcept {
        assertion(m_gp_temp1.has_value(), "Must be");
        return m_gp_temp1.value();
    }

    [[nodiscard]]
    aasm::GPReg gp_temp2() const noexcept {
        assertion(m_gp_temp2.has_value(), "Must be");
        return m_gp_temp2.value();
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return !m_gp_temp1.has_value() && !m_gp_temp2.has_value();
    }

private:
    std::optional<aasm::GPReg> m_gp_temp1{};
    std::optional<aasm::GPReg> m_gp_temp2{};
};