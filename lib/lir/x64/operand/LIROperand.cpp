#include "LIROperand.h"
#include "base/Constant.h"

#include <ostream>

LIRValType LIROperand::type() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) -> LIRValType {
        if constexpr (std::is_same_v<T, const LIRNamedSlot*> || std::is_same_v<T, LirCst>) {
            return LIRValType::GP;

        } else if constexpr (std::is_same_v<T, LIRVal>) {
            return val.type();

        } else {
            static_assert(false);
            std::unreachable();
        }
    };

    return std::visit(visitor, m_operand);
}

std::uint8_t LIROperand::align() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) -> std::uint8_t {
        if constexpr (std::is_same_v<T, const LIRNamedSlot*>) {
            return cst::POINTER_SIZE;

        } else if constexpr (std::is_same_v<T, LIRVal>) {
            return val.alignment();

        } else if constexpr (std::is_same_v<T, LirCst>) {
            return val.size();

        } else {
            static_assert(false);
            std::unreachable();
        }
    };

    return std::visit(visitor, m_operand);
}

std::ostream & operator<<(std::ostream &os, const LIROperand &op) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, const LIRNamedSlot*>) {
            os << *val;
        } else {
            os << val;
        }
    };

    std::visit(visitor, op.m_operand);
    return os;
}
