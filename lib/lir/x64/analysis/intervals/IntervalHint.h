#pragma once

#include <cstdint>

enum class IntervalHint: std::uint8_t {
    NOTHING = 0,
    CALL_LIVE_OUT
};

static IntervalHint join_hints(const IntervalHint lhs, const IntervalHint rhs) noexcept {
    if (lhs == IntervalHint::CALL_LIVE_OUT && rhs == IntervalHint::CALL_LIVE_OUT) {
        return IntervalHint::CALL_LIVE_OUT;
    }

    return IntervalHint::NOTHING;
}