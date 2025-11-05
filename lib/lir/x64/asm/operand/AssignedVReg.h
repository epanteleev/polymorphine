#pragma once

#include <variant>

#include "GPVReg.h"
#include "XVReg.h"
#include "asm/x64/asm.h"

class AssignedVReg final {
public:
    template<typename T>
    AssignedVReg(const T& reg) noexcept:
        m_reg(reg) {}

    AssignedVReg(const GPVReg& reg) noexcept {
        const auto visitor = [&]<typename T>(const T &val) { m_reg = val; };
        reg.visit(visitor);
    }

    explicit AssignedVReg() noexcept: m_reg(std::monostate{}) {}

    [[nodiscard]]
    bool empty() const noexcept {
        return std::holds_alternative<std::monostate>(m_reg);
    }

    [[nodiscard]]
    std::optional<aasm::Reg> to_reg() const noexcept {
        const auto visitor = [&]<typename T>(const T &val) -> std::optional<aasm::Reg> {
            if constexpr (std::is_same_v<T, aasm::GPReg> || std::is_same_v<T, aasm::XmmReg>) {
                return val;
            }

            return std::nullopt;
        };

        return std::visit(visitor, m_reg);
    }

    [[nodiscard]]
    std::optional<GPVReg> to_gp_op() const noexcept {
        const auto visitor = [&]<typename T>(const T &val) -> std::optional<GPVReg> {
            if constexpr (GPVRegVariant<T>) {
                return GPVReg(val);
            }

            return std::nullopt;
        };

        return std::visit(visitor, m_reg);
    }

    [[nodiscard]]
    std::optional<XVReg> to_xmm_op() const noexcept {
        const auto visitor = [&]<typename T>(const T &val) -> std::optional<XVReg> {
            if constexpr (XVRegVariant<T>) {
                return XVReg(val);
            }

            return std::nullopt;
        };

        return std::visit(visitor, m_reg);
    }

private:
    std::variant<std::monostate, aasm::GPReg, aasm::XmmReg, aasm::Address> m_reg;
};
