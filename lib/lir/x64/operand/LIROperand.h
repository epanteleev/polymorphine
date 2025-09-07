#pragma once

#include <optional>
#include <variant>

#include "LIRCst.h"
#include "LIRVal.h"

class LIROperand final {
public:
    constexpr LIROperand(const LirCst& operand) noexcept:
        m_operand(operand) {}

    constexpr LIROperand(const LIRVal& operand) noexcept:
        m_operand(operand) {}

    [[nodiscard]]
    constexpr std::optional<LirCst> as_cst() const noexcept {
        if (std::holds_alternative<LirCst>(m_operand)) {
            return std::get<LirCst>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<LIRVal> as_vreg() const noexcept {
        if (std::holds_alternative<LIRVal>(m_operand)) {
            return std::get<LIRVal>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    constexpr std::uint8_t size() const noexcept {
        const auto visitor = [&]<typename T>(const T &val) {
            return val.size();
        };

        return std::visit(visitor, m_operand);
    }

    friend std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;
private:
    std::variant<LirCst, LIRVal> m_operand;
};


std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;