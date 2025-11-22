#pragma once

#include <cstdint>
#include <iosfwd>
#include <utility>

namespace aasm {
    enum class CondType: std::uint8_t {
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

    std::ostream& operator<<(std::ostream &os, const CondType &cond);

    constexpr CondType invert(const CondType cond) noexcept {
        if (const auto code = static_cast<std::uint8_t>(cond); (code & 1) == 0) {
            return static_cast<CondType>(code + 1);
        } else {
            return static_cast<CondType>(code - 1);
        }
    }
}