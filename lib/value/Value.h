#pragma once

#include <cstdint>
#include <variant>
#include <iosfwd>

#include "../ir_frwd.h"

template <typename T>
concept IsValueType = std::is_same_v<T, double> ||
    std::is_same_v<T, std::int64_t> ||
    std::is_same_v<T, std::uint64_t> ||
    std::is_same_v<T, ValueInstruction *>;

class Value final {
public:
    Value(double value, FloatingPointType* type) noexcept;
    Value(std::uint64_t value, UnsignedIntegerType* type) noexcept;
    Value(std::int64_t value, SignedIntegerType * type) noexcept;

    template <IsValueType T>
    [[nodiscard]] T get() const {
        return std::get<T>(m_value);
    }

    template <IsValueType T>
    [[nodiscard]]
    bool is() const {
        return std::holds_alternative<T>(m_value);
    }

    [[nodiscard]]
    Type* type() const noexcept {
        return m_type;
    }

    friend std::ostream& operator<<(std::ostream& os, const Value& obj);

    static Value i32(int value) noexcept;

private:
    std::variant<double,
        std::int64_t,
        std::uint64_t,
        ValueInstruction *> m_value;
    Type* m_type;
};

std::ostream& operator<<(std::ostream& os, const Value& obj);

static_assert(std::is_trivially_copyable_v<Value>, "assumed to be");