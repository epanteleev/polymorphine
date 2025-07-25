#include "LIRSetCC.h"
#include <ostream>

std::ostream & operator<<(std::ostream &os, const LIRCondType &cond) {
    switch (cond) {
        case LIRCondType::O:  return os << "o";
        case LIRCondType::NO: return os << "no";
        case LIRCondType::NAE:return os << "nae";
        case LIRCondType::AE: return os << "ae";
        case LIRCondType::E:  return os << "e";
        case LIRCondType::NE: return os << "ne";
        case LIRCondType::NA: return os << "na";
        case LIRCondType::A:  return os << "a";
        case LIRCondType::S:  return os << "s";
        case LIRCondType::NS: return os << "ns";
        case LIRCondType::P:  return os << "p";
        case LIRCondType::NP: return os << "np";
        case LIRCondType::NGE:return os << "nge";
        case LIRCondType::GE: return os << "ge";
        case LIRCondType::NG: return os << "ng";
        case LIRCondType::G:  return os << "g";
        default: std::unreachable();
    }
}

void LIRSetCC::visit(LIRVisitor &visitor) {
    visitor.setcc_i(def(0), m_cond_type, in(0));
}
