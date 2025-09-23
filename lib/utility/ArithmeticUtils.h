#pragma once

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

template<std::integral To, std::integral From >
constexpr To checked_cast(const From & from) {
    To result = To( from );
#ifndef NDEBUG
    From original{};
    std::memcpy(&original, &result, sizeof(To));
    assertion(std::in_range<To>(from) || std::memcmp(&original, &from, sizeof(From)) == 0, "Checked cast failed: {} cannot be safely converted. Result is {}", from, result);
#endif
    return result;
}