#pragma once

#include "asm/x64/asm.h"

namespace call_conv {
    static constexpr std::size_t STACK_ALIGNMENT = 16;

    class CallConvProvider final {
    public:
        explicit consteval CallConvProvider(const std::span<aasm::GPReg const> gp_argument_registers,
                                  const std::span<aasm::GPReg const> gp_caller_save_registers,
                                  const std::span<aasm::GPReg const> gp_callee_save_registers,
                                  const std::span<aasm::GPReg const> all_gp_registers) noexcept:
            m_gp_argument_registers(gp_argument_registers),
            m_gp_caller_save_registers(gp_caller_save_registers),
            m_gp_callee_save_registers(gp_callee_save_registers),
            m_all_gp_registers(all_gp_registers) {}

        [[nodiscard]]
        constexpr std::span<aasm::GPReg const> GP_ARGUMENT_REGISTERS() const noexcept {
            return m_gp_argument_registers;
        }

        [[nodiscard]]
        constexpr aasm::GPReg GP_ARGUMENT_REGISTERS(const std::size_t idx) const noexcept {
            return m_gp_argument_registers[idx];
        }

        [[nodiscard]]
        constexpr std::span<aasm::GPReg const> GP_CALLER_SAVE_REGISTERS() const noexcept {
            return m_gp_caller_save_registers;
        }

        [[nodiscard]]
        constexpr aasm::GPReg GP_CALLER_SAVE_REGISTERS(const std::size_t idx) const noexcept {
            return m_gp_caller_save_registers[idx];
        }

        [[nodiscard]]
        constexpr std::span<aasm::GPReg const> GP_CALLEE_SAVE_REGISTERS() const noexcept {
            return m_gp_callee_save_registers;
        }

        [[nodiscard]]
        constexpr aasm::GPReg GP_CALLEE_SAVE_REGISTERS(const std::size_t idx) const noexcept {
            return m_gp_callee_save_registers[idx];
        }

        [[nodiscard]]
        constexpr std::span<aasm::GPReg const> ALL_GP_REGISTERS() const noexcept {
            return m_all_gp_registers;
        }

        [[nodiscard]]
        constexpr aasm::GPReg ALL_GP_REGISTERS(const std::size_t idx) const noexcept {
            return m_all_gp_registers[idx];
        }

    private:
        std::span<aasm::GPReg const> m_gp_argument_registers;
        std::span<aasm::GPReg const> m_gp_caller_save_registers;
        std::span<aasm::GPReg const> m_gp_callee_save_registers;
        std::span<aasm::GPReg const> m_all_gp_registers;
    };
}