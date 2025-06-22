#pragma once
#include <cstdint>

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

private:
    std::int64_t m_value;
    Kind m_kind;
};
