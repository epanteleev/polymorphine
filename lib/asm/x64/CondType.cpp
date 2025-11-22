#include "CondType.h"

#include <ostream>
#include <utility>

namespace aasm {
    std::ostream& operator<<(std::ostream &os, const CondType &cond) {
        switch (cond) {
            case CondType::O:  return os << "o";
            case CondType::NO: return os << "no";
            case CondType::NAE:return os << "nae";
            case CondType::AE: return os << "ae";
            case CondType::E:  return os << "e";
            case CondType::NE: return os << "ne";
            case CondType::NA: return os << "na";
            case CondType::A:  return os << "a";
            case CondType::S:  return os << "s";
            case CondType::NS: return os << "ns";
            case CondType::P:  return os << "p";
            case CondType::NP: return os << "np";
            case CondType::NGE:return os << "nge";
            case CondType::GE: return os << "ge";
            case CondType::NG: return os << "ng";
            case CondType::G:  return os << "g";
            default: std::unreachable();
        }
    }
}