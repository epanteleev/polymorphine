#pragma once

#include "asm/x64/reg/Reg.h"
#include "lir/x64/analysis/intervals/IntervalHint.h"
#include "lir/x64/asm/cc/LinuxX64.h"
#include "lir/x64/asm/operand/AssignedVReg.h"
#include "lir/x64/operand/LIRValType.h"
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
                case LIRValType::GP: return top_gp(hint);
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

        void remove(aasm::Reg reg) noexcept;

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
        aasm::GPReg top_gp(IntervalHint hint) noexcept;

        aasm::XmmReg top_xmm(IntervalHint hint) noexcept;

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