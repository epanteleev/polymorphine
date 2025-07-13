#include "LIRCst.h"

#include <ostream>

std::ostream & operator<<(std::ostream &os, const LirCst &op) noexcept {
    os << op.m_value;
    switch (op.m_kind) {
        case LirCst::Kind::Int8: {
            os << "'b";
            break;
        }
        case LirCst::Kind::Int16: {
            os << "'w";
            break;
        }
        case LirCst::Kind::Int32: {
            os << "'d";
            break;
        }
        case LirCst::Kind::Int64: {
            os << "'q";
            break;
        }
    }
    return os;
}
