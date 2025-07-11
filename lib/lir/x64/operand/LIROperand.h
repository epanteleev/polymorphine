#pragma once
#include <optional>
#include <sstream>
#include <variant>

#include "LIRCst.h"
#include "LIRVal.h"

class LIROperand final {
public:
    LIROperand(LirCst operand) noexcept:
        m_operand(operand) {}

    LIROperand(LIRVal operand) noexcept:
        m_operand(operand) {}

    [[nodiscard]]
    std::optional<LirCst> cst() const noexcept {
        if (std::holds_alternative<LirCst>(m_operand)) {
            return std::get<LirCst>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<LIRVal> vreg() const noexcept {
        if (std::holds_alternative<LIRVal>(m_operand)) {
            return std::get<LIRVal>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;
private:
    std::variant<LirCst, LIRVal> m_operand;
};


std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;