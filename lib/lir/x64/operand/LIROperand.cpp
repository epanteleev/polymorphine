#include "LIROperand.h"
#include <ostream>

#include "base/Constant.h"

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
