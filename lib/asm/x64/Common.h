#pragma once

#include <cstdint>
#include <cstring>
#include <utility>

#include "utility/Error.h"

namespace aasm {
    namespace constants {
        static constexpr std::uint8_t REX = 0x40;
        static constexpr std::uint8_t REX_W = 0x48; // REX prefix for 64-bit operands
        static constexpr auto PREFIX_OPERAND_SIZE = 0x66;
        static constexpr std::uint8_t MAX_X86_INSTRUCTION_SIZE = 15; // Maximum size of an x86 instruction
        static constexpr auto NO_OFFSET = std::numeric_limits<std::uint32_t>::max();
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
        { t.patch32(std::declval<std::uint32_t>(), std::declval<std::uint32_t>())  };
        { t.patch64(std::declval<std::uint32_t>(), std::declval<std::uint64_t>()) };
        { t.size() } -> std::same_as<std::size_t>;
    };

    template<std::integral T, std::size_t... I>
    constexpr bool is_in_range(const T& size, const std::integer_sequence<std::size_t, I...>) {
        return ((size == I) || ...);
    }

    template<CodeBuffer Buffer>
    static constexpr void add_word_op_size(Buffer& buffer) {
        buffer.emit8(constants::PREFIX_OPERAND_SIZE);
    }

    template<std::integral To, std::integral From >
    constexpr static To checked_cast(const From & from) {
        To result = To( from );
        From original{};
        std::memcpy(&original, &result, sizeof(To));
        assertion(std::in_range<To>(from) || std::memcmp(&original, &from, sizeof(From)) == 0, "Checked cast failed: {} cannot be safely converted. Result is {}", from, result);
        return result;
    }

    [[maybe_unused]]
    static char prefix_size(std::uint8_t size) {
        switch (size) {
            case 1: return 'b';
            case 2: return 'w';
            case 4: return 'l';
            case 8: return 'q';
            default: die("Invalid size for prefix: {}", size);
        }
    }
}
