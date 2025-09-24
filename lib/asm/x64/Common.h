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
