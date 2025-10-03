#pragma once

#include "lir/x64/analysis/intervals/IntervalHint.h"
#include "lir/x64/asm/cc/LinuxX64.h"
#include "utility/ArithmeticUtils.h"

namespace details {
    class VRegSelection final {
    public:
        explicit VRegSelection(std::vector<aasm::GPReg>&& free_regs, const call_conv::CallConvProvider* call_conv) noexcept:
            m_free_gp_regs(std::move(free_regs)),
            m_call_conv(call_conv) {}

        aasm::Address stack_alloc(const std::size_t size, const std::size_t align) noexcept {
            m_local_area_size = align_up(m_local_area_size, align) + size;
            return aasm::Address(aasm::rbp, -m_local_area_size);
        }

        [[nodiscard]]
        aasm::Reg top(const IntervalHint hint, const LIRValType type) {
            switch (type) {
                case LIRValType::GP: return top(hint);
                case LIRValType::FP: return top_xmm(hint);
                default: std::unreachable();
            }
        }

        void try_push(const AssignedVReg& vreg) {
            const auto reg_opt = vreg.to_reg();
            if (!reg_opt.has_value()) {
                return;
            }
            push(reg_opt.value());
        }

        void push(const aasm::Reg reg) {
            const auto vis = [&]<typename T>(const T& val) {
                push(val);
            };

            reg.visit(vis);
        }

        void try_remove(const AssignedVReg& vreg) {
            const auto reg_opt = vreg.to_reg();
            if (!reg_opt.has_value()) {
                return;
            }
            remove(reg_opt.value());
        }

        void remove(const aasm::Reg reg) noexcept {
            const auto vis = [&]<typename T>(const T& val) {
                if constexpr (std::is_same_v<T, aasm::GPReg>) {
                    std::erase(m_free_gp_regs, reg);
                } else if constexpr (std::is_same_v<T, aasm::XmmReg>) {
                    std::erase(m_free_xmm_regs, reg);
                } else {
                    static_assert(false);
                    std::unreachable();
                }
            };
            reg.visit(vis);
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return m_free_gp_regs.empty();
        }

        [[nodiscard]]
        std::int32_t local_area_size() const noexcept {
            return m_local_area_size;
        }

        template<std::ranges::range Range>
        static VRegSelection create(const call_conv::CallConvProvider* call_conv, Range&& arg_regs) {
            std::vector<aasm::GPReg> regs{};
            regs.reserve(call_conv->ALL_GP_REGISTERS().size());

            for (const auto reg: call_conv->ALL_GP_REGISTERS()) {
                if (std::ranges::contains(arg_regs, reg)) {
                    continue;
                }

                regs.push_back(reg);
            }

            return VRegSelection(std::move(regs), call_conv);
        }

    private:
        [[nodiscard]]
        aasm::GPReg top(const IntervalHint hint) noexcept {
            switch (hint) {
                case IntervalHint::NOTHING: {
                    for (const auto reg: std::ranges::reverse_view(m_free_gp_regs)) {
                        if (!std::ranges::contains(m_call_conv->GP_CALLER_SAVE_REGISTERS(), reg)) {
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

        aasm::XmmReg top_xmm(const IntervalHint hint) {
            switch (hint) {
                case IntervalHint::NOTHING: {
                    for (const auto reg: std::ranges::reverse_view(m_free_xmm_regs)) {
                        if (!std::ranges::contains(m_call_conv->XMM_CALLER_SAVE_REGISTERS(), reg)) {
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

        void push(const aasm::GPReg reg) noexcept {
            if (std::ranges::contains(m_free_gp_regs, reg)) {
                return;
            }

            m_free_gp_regs.push_back(reg);
        }

        void push(const aasm::XmmReg reg) noexcept {
            if (std::ranges::contains(m_free_xmm_regs, reg)) {
                return;
            }

            m_free_xmm_regs.push_back(reg);
        }

        std::vector<aasm::GPReg> m_free_gp_regs{};
        std::vector<aasm::XmmReg> m_free_xmm_regs{};
        std::int32_t m_local_area_size{};
        const call_conv::CallConvProvider* m_call_conv;
    };
}