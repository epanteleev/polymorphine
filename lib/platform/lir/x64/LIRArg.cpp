#include "LIRArg.h"

#include <ostream>

std::ostream & operator<<(std::ostream &os, const LIRArg &op) noexcept {
    os << op.m_size << '[' << op.m_index << ']';
    return os;
}
