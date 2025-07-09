#include "LIRCst.h"

#include <ostream>

std::ostream & operator<<(std::ostream &os, const LirCst &op) noexcept {
    switch (op.m_kind) {
        case LirCst::Kind::Int8: {
            os << "imm8[";
            break;
        }
        case LirCst::Kind::Int16: {
            os << "imm16[";
            break;
        }
        case LirCst::Kind::Int32: {
            os << "imm32[";
            break;
        }
        case LirCst::Kind::Int64: {
            os << "imm64[";
            break;
        }
    }

    os << op.m_value << ']';
    return os;
}
