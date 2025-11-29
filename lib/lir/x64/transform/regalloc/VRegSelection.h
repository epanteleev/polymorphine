#pragma once

#include "asm/x64/reg/Reg.h"
#include "lir/x64/analysis/intervals/IntervalHint.h"
#include "lir/x64/asm/TemporalRegs.h"
#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/asm/operand/AssignedVReg.h"
#include "lir/x64/operand/LIRValType.h"

namespace details {
    class VRegSelection final {
    public:
        explicit VRegSelection(InplaceVec<aasm::GPReg, aasm::GPReg::NUMBER_OF_REGISTERS>&& free_gp_regs,
            InplaceVec<aasm::XmmReg, aasm::XmmReg::NUMBER_OF_REGISTERS>&& free_xmm_regs, const call_conv::CallConvProvider* call_conv) noexcept:
            m_free_gp_regs(std::move(free_gp_regs)),
            m_free_xmm_regs(std::move(free_xmm_regs)),
            m_call_conv(call_conv) {}

        aasm::Address stack_alloc(const std::size_t size, const std::size_t align) noexcept {
            m_local_area_size = align_up(m_local_area_size, align) + size;
            return aasm::Address(aasm::rbp, -m_local_area_size);
        }

        [[nodiscard]]
        aasm::Reg top(const IntervalHint hint, const LIRValType type) noexcept {
            switch (type) {
                case LIRValType::GP: return top_gp(hint);
                case LIRValType::FP: return top_xmm(hint);
                default: std::unreachable();
            }
        }

        [[nodiscard]]
        aasm::GPReg top_gp(IntervalHint hint) noexcept;

        [[nodiscard]]
        aasm::XmmReg top_xmm(IntervalHint hint) noexcept;

        [[nodiscard]]
        InplaceVec<aasm::GPReg, TemporalRegs::MAX_NOF_GP_TEMPORAL_REGS> alloc_gp_temp(const aasm::RegSet& exclude, std::size_t nof_temps) const noexcept;

        [[nodiscard]]
        InplaceVec<aasm::XmmReg, TemporalRegs::MAX_NOF_XMM_TEMPORAL_REGS> alloc_xmm_temp(const aasm::RegSet& exclude, std::size_t nof_temps) const noexcept;

        void try_push(const AssignedVReg& vreg) {
            const auto reg_opt = vreg.to_reg();
            if (!reg_opt.has_value()) {
                return;
            }
            push(reg_opt.value());
        }

        void push(const aasm::Reg reg) {
            const auto vis = [&]<typename T>(const T& val) {
                push_impl(val);
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

        static VRegSelection create(const call_conv::CallConvProvider* call_conv, const aasm::RegSet &arg_regs);

    private:
        void push_impl(aasm::GPReg reg) noexcept;
        void push_impl(aasm::XmmReg reg) noexcept;

        InplaceVec<aasm::GPReg, aasm::GPReg::NUMBER_OF_REGISTERS> m_free_gp_regs;
        InplaceVec<aasm::XmmReg, aasm::XmmReg::NUMBER_OF_REGISTERS> m_free_xmm_regs;
        std::int32_t m_local_area_size{};
        const call_conv::CallConvProvider* m_call_conv;
    };
}