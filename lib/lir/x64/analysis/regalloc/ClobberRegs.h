#pragma once

#include "asm/asm.h"

class ClobberRegs final {
public:
    explicit ClobberRegs(const aasm::GPReg gp_temp1, const aasm::GPReg gp_temp2) noexcept:
        m_gp_temp1(gp_temp1),
        m_gp_temp2(gp_temp2) {}

    [[nodiscard]]
    aasm::GPReg gp_temp1() const noexcept {
        return m_gp_temp1;
    }

    [[nodiscard]]
    aasm::GPReg gp_temp2() const noexcept {
        return m_gp_temp2;
    }

private:
    aasm::GPReg m_gp_temp1;
    aasm::GPReg m_gp_temp2;
};
