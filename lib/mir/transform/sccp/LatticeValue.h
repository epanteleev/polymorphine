#pragma once

#include <cstdint>
#include "mir/value/Value.h"

enum class LatticeKind : std::uint8_t {
    Unknown,
    Constant,
    Overdefined
};

class LatticeValue final {
    explicit constexpr LatticeValue(const LatticeKind& _kind, const Value& _value) noexcept:
        m_kind(_kind),
        m_value(_value) {}

public:
    consteval LatticeValue() noexcept:
        m_kind(LatticeKind::Unknown),
        m_value(Value::undefined()) {}

    [[nodiscard]]
    static consteval LatticeValue unknown() noexcept {
        return LatticeValue(LatticeKind::Unknown, Value::undefined());
    }

    [[nodiscard]]
    static consteval LatticeValue overdefined() noexcept {
        return LatticeValue(LatticeKind::Overdefined, Value::undefined());
    }

    [[nodiscard]]
    static LatticeValue constant(const Value& val) noexcept {
        return LatticeValue(LatticeKind::Constant, val);
    }

    [[nodiscard]]
    constexpr LatticeKind kind() const noexcept {
        return m_kind;
    }

    [[nodiscard]]
    constexpr const Value& value() const noexcept {
        return m_value;
    }

private:
    LatticeKind m_kind{};
    Value m_value;
};