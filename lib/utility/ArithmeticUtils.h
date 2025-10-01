#pragma once

#include <cstdint>
#include <concepts>
#include <cstring>
#include <utility>
#include "utility/Error.h"


template<std::integral T, std::integral U>
constexpr T align_up(const T value, const U alignment) noexcept {
    if (alignment == 0) {
        return value;
    }

    return (value + alignment - 1) / alignment * alignment;
}

template<std::unsigned_integral To, std::integral From >
constexpr To checked_cast(const From & from) {
    To result = To( from );
    assertion(from == static_cast<From>(result), "Checked cast failed: {} cannot be safely converted. Result is {}", from, result);
    return result;
}

template<std::signed_integral To, std::integral From >
constexpr To checked_cast(const From & from) {
    To result = To( from );
#ifndef NDEBUG
    From original{};
    std::memcpy(&original, &result, sizeof(To));
    assertion(std::in_range<To>(from) || std::memcmp(&original, &from, sizeof(From)) == 0, "Checked cast failed: {} cannot be safely converted. Result is {}", from, result);
#endif
    return result;
}

template<std::floating_point T>
constexpr std::int64_t bitcast(const T value) noexcept {
    std::int64_t result{};
    std::memcpy(&result, &value, sizeof(T));
    return result;
}

