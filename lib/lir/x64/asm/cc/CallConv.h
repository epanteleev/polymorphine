#pragma once

#include "asm/x64/asm.h"

namespace call_conv {
    static constexpr std::size_t STACK_ALIGNMENT = 16;

    class CallConvProvider final {
    public:
        explicit consteval CallConvProvider(const std::span<aasm::GPReg const> gp_argument_registers,
                                  const aasm::GPRegSet& gp_caller_save_registers,
                                  const aasm::GPRegSet& gp_callee_save_registers,
                                  const aasm::GPRegSet& all_gp_registers,
                                  const std::span<aasm::XmmReg const> xmm_argument_registers,
                                  const aasm::XmmRegSet& xmm_caller_save_registers,
                                  const aasm::XmmRegSet& xmm_callee_save_registers,
                                  const aasm::XmmRegSet& all_xmm_registers) noexcept:
            m_gp_argument_registers(gp_argument_registers),
            m_gp_caller_save_registers(gp_caller_save_registers),
            m_gp_callee_save_registers(gp_callee_save_registers),
            m_all_gp_registers(all_gp_registers),
            m_xmm_argument_registers(xmm_argument_registers),
            m_xmm_caller_save_registers(xmm_caller_save_registers),
            m_xmm_callee_save_registers(xmm_callee_save_registers),
            m_all_xmm_registers(all_xmm_registers) {}

        [[nodiscard]]
        constexpr std::span<aasm::GPReg const> GP_ARGUMENT_REGISTERS() const noexcept {
            return m_gp_argument_registers;
        }

        [[nodiscard]]
        constexpr aasm::GPReg GP_ARGUMENT_REGISTERS(const std::size_t idx) const noexcept {
            return m_gp_argument_registers[idx];
        }

        [[nodiscard]]
        constexpr std::span<aasm::XmmReg const> XMM_ARGUMENT_REGISTERS() const noexcept {
            return m_xmm_argument_registers;
        }

        [[nodiscard]]
        constexpr aasm::XmmReg XMM_ARGUMENT_REGISTERS(const std::size_t idx) const noexcept {
            return m_xmm_argument_registers[idx];
        }

        [[nodiscard]]
        constexpr const aasm::GPRegSet& GP_CALLER_SAVE_REGISTERS() const noexcept {
            return m_gp_caller_save_registers;
        }

        [[nodiscard]]
        constexpr const aasm::XmmRegSet& XMM_CALLER_SAVE_REGISTERS() const noexcept {
            return m_xmm_caller_save_registers;
        }

        [[nodiscard]]
        constexpr const aasm::GPRegSet& GP_CALLEE_SAVE_REGISTERS() const noexcept {
            return m_gp_callee_save_registers;
        }

        [[nodiscard]]
        constexpr const aasm::GPRegSet& ALL_GP_REGISTERS() const noexcept {
            return m_all_gp_registers;
        }

        [[nodiscard]]
        constexpr const aasm::XmmRegSet& ALL_XMM_REGISTERS() const noexcept {
            return m_all_xmm_registers;
        }

    private:
        std::span<aasm::GPReg const> m_gp_argument_registers;
        aasm::GPRegSet m_gp_caller_save_registers;
        aasm::GPRegSet m_gp_callee_save_registers;
        aasm::GPRegSet m_all_gp_registers;

        std::span<aasm::XmmReg const> m_xmm_argument_registers;
        aasm::XmmRegSet m_xmm_caller_save_registers;
        aasm::XmmRegSet m_xmm_callee_save_registers;
        aasm::XmmRegSet m_all_xmm_registers;
    };
}