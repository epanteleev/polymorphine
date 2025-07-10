#pragma once
#include <optional>
#include <variant>

#include "LIRCst.h"
#include "LIRVReg.h"

class LIROperand final {
public:
    LIROperand(LirCst operand) noexcept
    : m_operand(operand) {}

    LIROperand(LIRVReg operand) noexcept
        : m_operand(operand) {}

    [[nodiscard]]
    std::optional<LirCst> cst() const noexcept {
        if (std::holds_alternative<LirCst>(m_operand)) {
            return std::get<LirCst>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<LIRVReg> vreg() const noexcept {
        if (std::holds_alternative<LIRVReg>(m_operand)) {
            return std::get<LIRVReg>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;
private:
    std::variant<LirCst, LIRVReg> m_operand;
};


std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;
