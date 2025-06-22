#pragma once
#include <optional>
#include <variant>

#include "LIRCst.h"
#include "Vreg.h"

class LIROperand final {
    explicit LIROperand(LirCst operand) noexcept
        : m_operand(operand) {}

    explicit LIROperand(VReg operand) noexcept
        : m_operand(operand) {}

public:
    [[nodiscard]]
    std::optional<LirCst> cst() const noexcept {
        if (std::holds_alternative<LirCst>(m_operand)) {
            return std::get<LirCst>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<VReg> vreg() const noexcept {
        if (std::holds_alternative<VReg>(m_operand)) {
            return std::get<VReg>(m_operand);
        }

        return std::nullopt;
    }

private:
    std::variant<LirCst, VReg> m_operand;
};
