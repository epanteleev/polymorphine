#pragma once

#include <cstdint>
#include <utility>

#include "utility/Error.h"

namespace aasm {
    namespace constants {
        static constexpr std::uint8_t REX = 0x40;
        static constexpr std::uint8_t REX_W = 0x48; // REX prefix for 64-bit operands
        static constexpr auto PREFIX_OPERAND_SIZE = 0x66;
    }

    static char prefix_size(std::uint8_t size) {
        switch (size) {
            case 1: return 'b';
            case 2: return 'w';
            case 4: return 'l';
            case 8: return 'q';
            default: die("Invalid size for prefix: {}", size);
        }
    }

    template<typename T>
    static constexpr bool in_byte_range(T arg) {
        return arg >= -128 && arg <= 127;
    }

    template<typename T>
    concept CodeBuffer = requires(T t) {
        { t.emit8(std::declval<std::uint8_t>()) };
        { t.emit32(std::declval<std::uint32_t>()) };
        { t.size() } -> std::same_as<std::size_t>;
    };

    template<std::integral T, std::ranges::range R>
    constexpr bool is_in_range(const T& size, const R& range) {
        return range.contains(size);
    }
}
