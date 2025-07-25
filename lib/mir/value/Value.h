#pragma once

#include <cstdint>
#include <variant>
#include <iosfwd>

#include "mir/mir_frwd.h"
#include "mir/types/Type.h"

template <typename T>
concept IsValueType = std::is_same_v<T, double> ||
    std::is_same_v<T, std::int64_t> ||
    std::is_same_v<T, std::uint64_t> ||
    std::is_same_v<T, ArgumentValue *> ||
    std::is_same_v<T, ValueInstruction *>;

class Value final {
public:
    constexpr Value(double value, const FloatingPointType *type) noexcept: m_value(value), m_type(type) {}
    constexpr Value(std::uint64_t value, const UnsignedIntegerType* type) noexcept: m_value(value), m_type(type) {}
    constexpr Value(std::int64_t value, const SignedIntegerType * type) noexcept: m_value(value), m_type(type) {}

    Value(const ArgumentValue* value) noexcept;
    Value(ValueInstruction * value) noexcept;

    template <IsValueType T>
    [[nodiscard]] constexpr T get() const {
        return std::get<T>(m_value);
    }

    template <IsValueType T>
    [[nodiscard]]
    constexpr bool is() const {
        return std::holds_alternative<T>(m_value);
    }

    template<typename Matcher>
    constexpr bool isa(Matcher&& matcher) const noexcept {
        return matcher(*this);
    }

    [[nodiscard]]
    constexpr const Type* type() const noexcept {
        return m_type;
    }

    template <typename T, typename Visitor>
    constexpr T visit(Visitor&& visitor) const {
        return std::visit(std::forward<Visitor>(visitor), m_value);
    }

    friend std::ostream& operator<<(std::ostream& os, const Value& obj);

    constexpr static Value i8(std::int8_t value) noexcept {
        return {value, SignedIntegerType::i8()};
    }

    constexpr static Value u8(std::uint8_t value) noexcept {
        return {value, UnsignedIntegerType::u8()};
    }

    constexpr static Value i32(int value) noexcept {
        return {value, SignedIntegerType::i32()};
    }

    constexpr static Value u32(std::uint32_t value) noexcept {
        return {value, UnsignedIntegerType::u32()};
    }

    constexpr static Value i64(std::int64_t value) noexcept {
        return {value, SignedIntegerType::i64()};
    }

private:
    std::variant<double,
        std::int64_t,
        std::uint64_t,
        const ArgumentValue*,
        ValueInstruction *> m_value;
    const Type* m_type;
};

std::ostream& operator<<(std::ostream& os, const Value& obj);

static_assert(std::is_trivially_copyable_v<Value>, "assumed to be");