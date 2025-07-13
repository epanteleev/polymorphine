#pragma once

#include <cstdint>
#include <iosfwd>

#include "utility/Error.h"

class LirCst final {
    enum class Kind : std::uint8_t {
        Int8,
        Int16,
        Int32,
        Int64,
    };

    constexpr LirCst(const std::int64_t value, const Kind kind) noexcept
        : m_value(value), m_kind(kind) {}

public:
    [[nodiscard]]
    constexpr std::uint8_t size() const noexcept {
        switch (m_kind) {
            case Kind::Int8: return 1;
            case Kind::Int16: return 2;
            case Kind::Int32: return 4;
            case Kind::Int64: return 8;
        }

        die("unreachable");
    }

    [[nodiscard]]
    constexpr std::int64_t value() const noexcept {
        return m_value;
    }

    static constexpr LirCst imm8(std::int64_t value) noexcept {
        return {value, Kind::Int8};
    }

    static constexpr LirCst imm16(std::int64_t value) noexcept {
        return {value, Kind::Int16};
    }

    static constexpr LirCst imm32(std::int64_t value) noexcept {
        return {value, Kind::Int32};
    }

    static constexpr LirCst imm64(std::int64_t value) noexcept {
        return {value, Kind::Int64};
    }

    friend std::ostream& operator<<(std::ostream& os, const LirCst& op) noexcept;

private:
    std::int64_t m_value;
    Kind m_kind;
};


std::ostream& operator<<(std::ostream& os, const LirCst& op) noexcept;