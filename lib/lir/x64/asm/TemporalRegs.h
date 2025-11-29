#pragma once

#include "asm/x64/asm.h"
#include "utility/InplaceVec.h"

class TemporalRegs final {
public:
    static constexpr auto MAX_NOF_GP_TEMPORAL_REGS = 2;
    static constexpr auto MAX_NOF_XMM_TEMPORAL_REGS = 2;

    explicit TemporalRegs() noexcept = default;

    explicit TemporalRegs(const InplaceVec<aasm::GPReg, MAX_NOF_GP_TEMPORAL_REGS> gp_regs, const InplaceVec<aasm::XmmReg, MAX_NOF_XMM_TEMPORAL_REGS> xmm_temps) noexcept:
        m_gp_temps(gp_regs),
        m_xmm_temps(xmm_temps) {}

    [[nodiscard]]
    aasm::GPReg gp_temp1() const noexcept {
        assertion(!m_gp_temps.empty(), "Must be");
        return m_gp_temps[0];
    }

    [[nodiscard]]
    aasm::GPReg gp_temp2() const noexcept {
        assertion(m_gp_temps.size() >= 2, "Must be");
        return m_gp_temps[1];
    }

    [[nodiscard]]
    aasm::XmmReg xmm_temp1() const noexcept {
        assertion(!m_xmm_temps.empty(), "Must be");
        return m_xmm_temps[0];
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_gp_temps.empty() && m_xmm_temps.empty();
    }

private:
    InplaceVec<aasm::GPReg, MAX_NOF_GP_TEMPORAL_REGS> m_gp_temps;
    InplaceVec<aasm::XmmReg, MAX_NOF_XMM_TEMPORAL_REGS> m_xmm_temps;
};