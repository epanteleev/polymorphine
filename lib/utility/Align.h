#pragma once

#include <concepts>

template<std::integral T, std::integral U>
constexpr T align_up(const T value, const U alignment) noexcept {
    if (alignment == 0) {
        return value;
    }

    return (value + alignment - 1) / alignment * alignment;
}