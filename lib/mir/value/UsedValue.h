#pragma once

#include <expected>
#include <span>
#include <variant>

#include "mir/mir_frwd.h"

template<typename T>
concept IsLocalValueType = std::derived_from<T, ValueInstruction> ||
    std::derived_from<T, ArgumentValue>;

class UsedValue final {
    explicit UsedValue(ArgumentValue* value) noexcept;
    explicit UsedValue(ValueInstruction * value) noexcept;

public:
    template <IsLocalValueType T>
    [[nodiscard]] T* get() const {
        return std::get<T*>(m_value);
    }

    template <IsLocalValueType T>
    [[nodiscard]]
    bool is() const noexcept {
        return std::holds_alternative<T*>(m_value);
    }

    bool operator==(const UsedValue &other) const noexcept {
        return m_value == other.m_value;
    }

    void add_user(Instruction* user);

    [[nodiscard]]
    std::span<const Instruction* const> users() const noexcept;

    [[nodiscard]]
    static std::expected<UsedValue, Error> try_from(const Value& value);

    template <IsLocalValueType T>
    static UsedValue from(const T* val) noexcept {
        return UsedValue(const_cast<T*>(val));
    }

private:
    std::variant<
        ArgumentValue *,
        ValueInstruction *> m_value;
};