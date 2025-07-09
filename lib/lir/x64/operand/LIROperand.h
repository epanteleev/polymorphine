#pragma once
#include <optional>
#include <variant>

#include "LIRCst.h"
#include "Vreg.h"

class LIROperand final {
public:
    LIROperand(LirCst operand) noexcept
    : m_operand(operand) {}

    LIROperand(VReg operand) noexcept
        : m_operand(operand) {}

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

    [[nodiscard]]
    std::uint8_t size() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;
private:
    std::variant<LirCst, VReg> m_operand;
};


std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;
