#pragma once

#include "asm/asm.h"

class ClobberRegs final {
public:
    explicit ClobberRegs() noexcept = default;
    explicit ClobberRegs(const std::optional<aasm::GPReg>& gp_temp1) noexcept:
        m_gp_temp1(gp_temp1) {}

    explicit ClobberRegs(const std::optional<aasm::GPReg>& gp_temp1, const std::optional<aasm::GPReg>& gp_temp2) noexcept:
        m_gp_temp1(gp_temp1),
        m_gp_temp2(gp_temp2) {}

    [[nodiscard]]
    aasm::GPReg gp_temp1() const noexcept {
        return m_gp_temp1.value();
    }

    void set_gp_temp1(const aasm::GPReg reg) noexcept {
        assertion(!m_gp_temp1.has_value(), "gp_temp1 is already set");
        m_gp_temp1 = reg;
    }

    [[nodiscard]]
    aasm::GPReg gp_temp2() const noexcept {
        return m_gp_temp2.value();
    }

    void set_gp_temp2(const aasm::GPReg reg) noexcept {
        assertion(!m_gp_temp2.has_value(), "gp_temp2 is already set");
        m_gp_temp2 = reg;
    }

private:
    std::optional<aasm::GPReg> m_gp_temp1{};
    std::optional<aasm::GPReg> m_gp_temp2{};
};
