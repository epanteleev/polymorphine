#pragma once

#include <optional>
#include <variant>

#include "LIRCst.h"
#include "LIRVal.h"
#include "lir/x64/global/LIRNamedSlot.h"

class LIROperand final {
public:
    constexpr LIROperand(const LirCst& operand) noexcept:
        m_operand(operand) {}

    constexpr LIROperand(const LIRVal& operand) noexcept:
        m_operand(operand) {}

    constexpr LIROperand(const LIRNamedSlot* operand) noexcept:
        m_operand(operand) {}

    [[nodiscard]]
    std::optional<LirCst> as_cst() const noexcept {
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
    std::optional<const LIRNamedSlot*> as_slot() const noexcept {
        if (std::holds_alternative<const LIRNamedSlot*>(m_operand)) {
            return std::get<const LIRNamedSlot*>(m_operand);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        const auto visitor = [&]<typename T>(const T &val) {
            if constexpr (std::is_same_v<T, const LIRNamedSlot*>) {
                return val->size();
            } else {
                return val.size();
            }
        };

        return std::visit(visitor, m_operand);
    }

    [[nodiscard]]
    std::uint8_t align() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;

private:
    std::variant<LirCst, LIRVal, const LIRNamedSlot*> m_operand;
};


std::ostream& operator<<(std::ostream& os, const LIROperand& op) noexcept;