#pragma once

#include <cstdint>

enum class ShiftKind: std::uint8_t {
    SAL,
    SHR,
    SAR,
};