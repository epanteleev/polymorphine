#include <algorithm>
#include <utility>
#include <ranges>

#include "VRegSelection.h"
#include "asm/x64/reg/Reg.h"

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

    aasm::GPReg VRegSelection::alloc_gp_temp(const aasm::RegSet& exclude) noexcept {
        for (const auto reg: std::ranges::reverse_view(m_free_gp_regs)) {
            if (exclude.contains(reg)) {
                continue;
            }
            if (!m_call_conv->GP_CALLER_SAVE_REGISTERS().contains(reg)) {
                continue;
            }

            remove(reg);
            return reg;
        }

        die("Cannot find a free register");
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

    aasm::XmmReg VRegSelection::alloc_xmm_temp(const aasm::RegSet& exclude) noexcept {
        for (const auto reg: std::ranges::reverse_view(m_free_xmm_regs)) {
            if (exclude.contains(reg)) {
                continue;
            }
            if (!m_call_conv->XMM_CALLER_SAVE_REGISTERS().contains(reg)) {
                continue;
            }

            remove(reg);
            return reg;
        }

        die("Cannot find a free register");
    }

    void VRegSelection::remove(const aasm::Reg reg) noexcept {
        const auto vis = [&]<typename T>(const T& val) {
            if constexpr (std::is_same_v<T, aasm::GPReg>) {
                std::erase(m_free_gp_regs, val);
            } else if constexpr (std::is_same_v<T, aasm::XmmReg>) {
                std::erase(m_free_xmm_regs, val);
            } else {
                static_assert(false);
                std::unreachable();
            }
        };
        reg.visit(vis);
    }

    template<typename Reg, std::size_t RS>
    static std::vector<Reg> collect_used_argument_regs(const aasm::AnyRegSet<Reg, RS>& all_registers, const aasm::RegSet &gp_arg_regs) {
        std::vector<Reg> regs{};
        regs.reserve(all_registers.size());
        for (const auto reg: all_registers) {
            if (gp_arg_regs.contains(reg)) {
                continue;
            }

            regs.push_back(reg);
        }

        return regs;
    }

    VRegSelection VRegSelection::create(const call_conv::CallConvProvider *call_conv, const aasm::RegSet &arg_regs) {
        auto gp_regs = collect_used_argument_regs(call_conv->ALL_GP_REGISTERS(), arg_regs);
        auto xmm_regs = collect_used_argument_regs(call_conv->ALL_XMM_REGISTERS(), arg_regs);

        return VRegSelection(std::move(gp_regs), std::move(xmm_regs), call_conv);
    }
}