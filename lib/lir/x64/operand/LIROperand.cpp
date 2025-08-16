#include "LIROperand.h"
#include <ostream>

std::ostream & operator<<(std::ostream &os, const LIROperand &op) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        os << val;
    };

    std::visit(visitor, op.m_operand);
    return os;
}
