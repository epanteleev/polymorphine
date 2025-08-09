#pragma once

#include <cstdint>
#include <ostream>
#include <utility>

enum class LIRCondType: std::uint8_t {
    O   = 0x0, // Overflow
    NO  = 0x1, // No Overflow
    NAE = 0x2, // Not Above or Equal (Below)
    AE  = 0x3, // Above or Equal (Not Below)
    E   = 0x4, // Equal (Zero)
    NE  = 0x5, // Not Equal (Not Zero)
    NA  = 0x6, // Not Above (Below or Equal)
    A   = 0x7, // Above (Not Below or Equal)
    S   = 0x8, // Sign (Negative)
    NS  = 0x9, // Not Sign (Positive)
    P   = 0xA, // Parity (Even)
    NP  = 0xB, // Not Parity (Odd)
    NGE = 0xC, // Not Greater or Equal (Less)
    GE  = 0xD, // Greater or Equal (Not Less)
    NG  = 0xE, // Not Greater (Less or Equal)
    G   = 0xF  // Greater (Not Less or Equal)
};

inline std::ostream & operator<<(std::ostream &os, const LIRCondType &cond) {
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