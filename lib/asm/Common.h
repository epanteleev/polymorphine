#pragma once

#include <cstdint>
#include <utility>

#include "utility/Error.h"

namespace aasm {
    namespace constants {
        static constexpr std::uint8_t REX = 0x40;
        static constexpr std::uint8_t REX_W = 0x48; // REX prefix for 64-bit operands
        static constexpr auto PREFIX_OPERAND_SIZE = 0x66;
        static constexpr std::uint8_t MAX_X86_INSTRUCTION_SIZE = 15; // Maximum size of an x86 instruction
    }

    template<std::integral T>
    constexpr bool in_size_range(T arg, std::size_t size) {
        switch (size) {
            case 1: return std::in_range<std::int8_t>(arg); // int8_t
            case 2: return std::in_range<std::int16_t>(arg); // int16_t
            case 4: return std::in_range<std::int32_t>(arg); // int32_t
            case 8: return true; // int64_t
            default: die("Invalid size for in_size_range: {}", size);
        }
    }

    template<typename T>
    concept CodeBuffer = requires(T t) {
        { t.emit8(std::declval<std::uint8_t>()) };
        { t.emit16(std::declval<std::uint16_t>()) };
        { t.emit32(std::declval<std::uint32_t>()) };
        { t.emit64(std::declval<std::uint64_t>()) };
        { t.size() } -> std::same_as<std::size_t>;
    };

    template<std::integral T, std::size_t... I>
    constexpr bool is_in_range(const T& size, const std::integer_sequence<std::size_t, I...>) {
        return ((size == I) || ...);
    }
}
