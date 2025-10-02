#pragma once

#include <cstdint>
#include <iosfwd>

/**
 * Represents a constant value in the LIR (Low-Level Intermediate Representation).
 * This class encapsulates integer constants of various sizes (8, 16, 32, and 64 bits).
 */
class LirCst final {
    enum class Kind : std::uint8_t {
        Int8 = 1,
        Int16 = 2,
        Int32 = 4,
        Int64 = 8
    };

    constexpr LirCst(const std::int64_t value, const Kind kind) noexcept
        : m_value(value), m_kind(kind) {}

public:
    [[nodiscard]]
    constexpr std::size_t size() const noexcept {
        return static_cast<std::size_t>(m_kind);
    }

    [[nodiscard]]
    constexpr std::int64_t value() const noexcept {
        return m_value;
    }

    static constexpr LirCst imm8(const std::int64_t value) noexcept {
        return {value, Kind::Int8};
    }

    static constexpr LirCst imm16(const std::int64_t value) noexcept {
        return {value, Kind::Int16};
    }

    static constexpr LirCst imm32(const std::int64_t value) noexcept {
        return {value, Kind::Int32};
    }

    static constexpr LirCst imm64(const std::int64_t value) noexcept {
        return {value, Kind::Int64};
    }

    static constexpr LirCst imm64(const std::uint64_t value) noexcept {
        return {static_cast<std::int64_t>(value), Kind::Int64};
    }

    friend std::ostream& operator<<(std::ostream& os, const LirCst& op) noexcept;

private:
    std::int64_t m_value;
    Kind m_kind;
};