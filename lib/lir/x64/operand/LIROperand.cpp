#include "LIROperand.h"
#include <ostream>

std::ostream & operator<<(std::ostream &os, const LIROperand &op) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, NamedSlot*>) {
            os << *val;
        } else {
            os << val;
        }
    };

    std::visit(visitor, op.m_operand);
    return os;
}
