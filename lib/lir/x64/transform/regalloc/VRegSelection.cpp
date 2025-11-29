#include <utility>
#include <algorithm>

#include "VRegSelection.h"

namespace details {
    aasm::GPReg VRegSelection::top_gp(const IntervalHint hint) noexcept {
        switch (hint) {
            case IntervalHint::NOTHING: {
                for (const auto reg: std::ranges::reverse_view(m_free_gp_regs)) {
                    if (!m_call_conv->GP_CALLER_SAVE_REGISTERS().contains(reg)) {
                        continue;
                    }

                    remove(reg);
                    return reg;
                }

                [[fallthrough]];
            }
            case IntervalHint::CALL_LIVE_OUT: {
                assertion(!m_free_gp_regs.empty(), "Attempted to access top of an empty register set");
                const auto reg = m_free_gp_regs.back();
                m_free_gp_regs.pop_back();
                return reg;
            }
            default: std::unreachable();
        }
    }

    aasm::XmmReg VRegSelection::top_xmm(const IntervalHint hint) noexcept {
        switch (hint) {
            case IntervalHint::NOTHING: {
                for (const auto reg: std::ranges::reverse_view(m_free_xmm_regs)) {
                    if (!m_call_conv->XMM_CALLER_SAVE_REGISTERS().contains(reg)) {
                        continue;
                    }

                    remove(reg);
                    return reg;
                }

                [[fallthrough]];
            }
            case IntervalHint::CALL_LIVE_OUT: {
                assertion(!m_free_xmm_regs.empty(), "Attempted to access top of an empty register set");
                const auto reg = m_free_xmm_regs.back();
                m_free_xmm_regs.pop_back();
                return reg;
            }
            default: std::unreachable();
        }
    }

    InplaceVec<aasm::GPReg, TemporalRegs::MAX_NOF_GP_TEMPORAL_REGS> VRegSelection::alloc_gp_temp(const aasm::RegSet& exclude, const std::size_t nof_temps) const noexcept {
        if (nof_temps == 0) return {};

        InplaceVec<aasm::GPReg, TemporalRegs::MAX_NOF_GP_TEMPORAL_REGS> temporals;
        std::size_t nof_allocated_regs{};
        for (const auto reg: std::ranges::reverse_view(m_free_gp_regs)) {
            if (exclude.contains(reg)) {
                continue;
            }
            if (!m_call_conv->GP_CALLER_SAVE_REGISTERS().contains(reg)) {
                continue;
            }

            temporals.push_back(reg);
            nof_allocated_regs+=1;
            if (nof_allocated_regs == nof_temps) {
                return temporals;
            }
        }
        die("Cannot find a free register");
    }

    InplaceVec<aasm::XmmReg, TemporalRegs::MAX_NOF_XMM_TEMPORAL_REGS> VRegSelection::alloc_xmm_temp(const aasm::RegSet& exclude, const std::size_t nof_temps) const noexcept {
        if (nof_temps == 0) return {};

        InplaceVec<aasm::XmmReg, TemporalRegs::MAX_NOF_XMM_TEMPORAL_REGS> temporals;
        std::size_t nof_allocated_regs{};
        for (const auto reg: std::ranges::reverse_view(m_free_xmm_regs)) {
            if (exclude.contains(reg)) {
                continue;
            }
            if (!m_call_conv->XMM_CALLER_SAVE_REGISTERS().contains(reg)) {
                continue;
            }

            temporals.push_back(reg);
            nof_allocated_regs+=1;
            if (nof_allocated_regs == nof_temps) {
                return temporals;
            }
        }

        die("Cannot find a free register");
    }

    void VRegSelection::remove(const aasm::Reg reg) noexcept {
        const auto vis = [&]<typename T>(const T& val) {
            if constexpr (std::is_same_v<T, aasm::GPReg>) {
                m_free_gp_regs.remove(val);
            } else if constexpr (std::is_same_v<T, aasm::XmmReg>) {
                m_free_xmm_regs.remove(val);
            } else {
                static_assert(false);
                std::unreachable();
            }
        };
        reg.visit(vis);
    }

    static InplaceVec<aasm::GPReg, aasm::GPReg::NUMBER_OF_REGISTERS> collect_used_gp_argument_regs(const call_conv::CallConvProvider *call_conv, const aasm::RegSet &gp_arg_regs) {
        InplaceVec<aasm::GPReg, aasm::GPReg::NUMBER_OF_REGISTERS> regs;
        for (const auto reg: call_conv->ALL_GP_REGISTERS()) {
            if (gp_arg_regs.contains(reg)) continue;
            regs.push_back(reg);
        }
        return regs;
    }

    static InplaceVec<aasm::XmmReg, aasm::XmmReg::NUMBER_OF_REGISTERS> collect_used_xmm_argument_regs(const call_conv::CallConvProvider *call_conv, const aasm::RegSet &gp_arg_regs) {
        InplaceVec<aasm::XmmReg, aasm::XmmReg::NUMBER_OF_REGISTERS> regs;
        for (const auto reg: call_conv->ALL_XMM_REGISTERS()) {
            if (gp_arg_regs.contains(reg)) continue;
            regs.push_back(reg);
        }
        return regs;
    }

    VRegSelection VRegSelection::create(const call_conv::CallConvProvider *call_conv, const aasm::RegSet &arg_regs) {
        auto gp_regs = collect_used_gp_argument_regs(call_conv, arg_regs);
        auto xmm_regs = collect_used_xmm_argument_regs(call_conv, arg_regs);
        return VRegSelection(std::move(gp_regs), std::move(xmm_regs), call_conv);
    }

    void VRegSelection::push_impl(const aasm::GPReg reg) noexcept {
        if (std::ranges::contains(m_free_gp_regs, reg)) {
            return;
        }

        m_free_gp_regs.push_back(reg);
    }

    void VRegSelection::push_impl(const aasm::XmmReg reg) noexcept {
        if (std::ranges::contains(m_free_xmm_regs, reg)) {
            return;
        }

        m_free_xmm_regs.push_back(reg);
    }
}