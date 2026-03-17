#pragma once

#include <cstdint>
#include <variant>
#include <iosfwd>
#include <utility>

#include "mir/mir_frwd.h"
#include "mir/types/FlagType.h"
#include "mir/types/Type.h"
#include "mir/types/FloatingPointType.h"
#include "mir/types/IntegerType.h"
#include "mir/types/Undef.h"

template <typename T>
concept IsValueType = std::is_same_v<T, double> ||
    std::is_same_v<T, std::int64_t> ||
    std::is_same_v<T, bool> ||
    std::is_same_v<T, ArgumentValue *> ||
    std::is_same_v<T, ValueInstruction *> ||
    std::is_same_v<T, GlobalValue *>;

class Value final {
public:
    using Variants = std::variant<double,
        std::int64_t,
        ArgumentValue*,
        bool,
        ValueInstruction *,
        GlobalValue*,
        std::monostate>;

private:
    constexpr Value(double value, const FloatingPointType *type) noexcept:
        m_value(value),
        m_type(type) {}

    constexpr Value(std::int64_t value, const IntegerType * type) noexcept:
        m_value(value),
        m_type(type) {}

    constexpr Value() noexcept:
        m_value(std::monostate{}),
        m_type(Undef::undef()) {}

    explicit Value(const VConstant& cst) noexcept;

    constexpr explicit Value(const bool value) noexcept:
        m_value(value),
        m_type(FlagType::flag()) {}

public:
    Value(const ArgumentValue* value) noexcept;
    Value(const ValueInstruction* value) noexcept;
    Value(const GlobalValue* value) noexcept;

    template <IsValueType T>
    [[nodiscard]]
    constexpr T get() const {
        return std::get<T>(m_value);
    }

    template <IsValueType T>
    [[nodiscard]]
    constexpr bool is() const {
        return std::holds_alternative<T>(m_value);
    }

    template<typename Matcher>
    [[nodiscard]]
    constexpr bool isa(Matcher&& matcher) const noexcept {
        return matcher(*this);
    }

    [[nodiscard]]
    constexpr const Type* type() const noexcept {
        return m_type;
    }

    template<std::derived_from<Type> T>
    [[nodiscard]]
    constexpr const T *as_type() const noexcept {
        if (const auto ty = T::cast(m_type)) {
            return ty;
        }

        std::unreachable();
    }

    template <typename Visitor>
    constexpr decltype(auto) visit(Visitor&& visitor) const {
        return std::visit(std::forward<Visitor>(visitor), m_value);
    }

    friend bool operator==(const Value& b, const Value& a) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const Value& obj);

    constexpr static Value i8(std::int8_t value) noexcept {
        return {value, SignedIntegerType::i8()};
    }

    constexpr static Value u8(std::uint8_t value) noexcept {
        return {value, UnsignedIntegerType::u8()};
    }

    constexpr static Value i16(std::int16_t value) noexcept {
        return {value, SignedIntegerType::i16()};
    }

    constexpr static Value u16(const std::uint16_t value) noexcept {
        return {value, UnsignedIntegerType::u16()};
    }

    constexpr static Value i32(const int value) noexcept {
        return {value, SignedIntegerType::i32()};
    }

    constexpr static Value u32(const std::uint32_t value) noexcept {
        return {value, UnsignedIntegerType::u32()};
    }

    constexpr static Value i64(const std::int64_t value) noexcept {
        return {value, SignedIntegerType::i64()};
    }

    constexpr static Value u64(const std::uint64_t value) noexcept {
        return {static_cast<std::int64_t>(value), UnsignedIntegerType::u64()};
    }

    constexpr static Value f64(const double value) noexcept {
        return {value, FloatingPointType::f64()};
    }

    constexpr static Value f32(const float value) noexcept {
        return {value, FloatingPointType::f32()};
    }

    constexpr static Value undefined() noexcept {
        return {};
    }

    constexpr static Value false_value() noexcept {
        return Value(false);
    }

    constexpr static Value true_value() noexcept {
        return Value(true);
    }

private:
    Variants m_value;
    const Type* m_type{};
};

namespace impls {
    inline bool any_value(const Value&) noexcept {
        return true;
    }

    inline bool constant(const Value& value) noexcept {
        return value.is<double>() || value.is<std::int64_t>();
    }

    inline bool signed_v(const Value& value) noexcept {
        return signed_type(value.type());
    }

    inline bool unsigned_v(const Value& value) noexcept {
        return unsigned_type(value.type());
    }

    inline bool integral(const Value& value, const std::uint64_t cst) noexcept {
        if (value.is<std::int64_t>()) {
            return value.get<std::int64_t>() == static_cast<std::int64_t>(cst);
        }

        return false;
    }
}

consteval auto any_value() noexcept {
    return impls::any_value;
}

consteval auto constant() {
    return impls::constant;
}

consteval auto float_type() noexcept {
    return impls::float_type;
}

consteval auto signed_v() {
    return impls::signed_v;
}

consteval auto unsigned_v() {
    return impls::unsigned_v;
}

consteval auto integral(const std::uint64_t cst) noexcept {
    return [=](const Value& value) { return impls::integral(value, cst); };
}