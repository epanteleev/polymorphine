#pragma once

#include <cstdint>
#include <string_view>

enum class FcmpOrdering : std::uint8_t {
    ORDERED,
    UNORDERED,
};

constexpr std::string_view to_string(const FcmpOrdering ord) noexcept {
    switch (ord) {
        case FcmpOrdering::UNORDERED: return "unordered";
        case FcmpOrdering::ORDERED:   return "ordered";
        default: std::unreachable();
    }
}