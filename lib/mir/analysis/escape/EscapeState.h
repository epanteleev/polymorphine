#pragma once

#include <cstdint>

enum class EscapeState: std::uint8_t {
    NOESCAPE,      // The value isn't escaped to memory, global context or another function.
    FIELD_ESCAPED, // The value is stored to memory
    ARGUMENT_ESCAPED,
    UNKNOWN,
};

[[nodiscard]]
inline EscapeState join(const EscapeState lhs, const EscapeState rhs) noexcept {
    if (lhs == rhs) {
        return lhs;
    }
    if (lhs == EscapeState::UNKNOWN || rhs == EscapeState::UNKNOWN) {
        return EscapeState::UNKNOWN;
    }

    if (lhs == EscapeState::NOESCAPE) return rhs;
    if (rhs == EscapeState::NOESCAPE) return lhs;

    return EscapeState::UNKNOWN;
}