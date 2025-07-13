#pragma once

#include <optional>
#include <variant>

#include "LIRCst.h"
#include "LIRVal.h"

class LIROperand final {
public:
    constexpr LIROperand(LirCst operand) noexcept:
        m_operand(operand) {}

    constexpr LIROperand(LIRVal operand) noexcept:
        m_operand(operand) {}

    [[nodiscard]]
    constexpr std::optional<LirCst> cst() const noexcept {
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
    constexpr std::uint8_t size() const noexcept {
        const auto visitor = [&]<typename T>(const T &val) {
            if constexpr (std::is_same_v<T, LirCst> || std::is_same_v<T, LIRVal>) {
                return val.size();
            } else {
                static_assert(false, "Unsupported type in Value variant");
            }
        };

        return std::visit(visitor, m_operand);
    }

    friend std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;
private:
    std::variant<LirCst, LIRVal> m_operand;
};


std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;