#pragma once

#include <expected>
#include <iosfwd>
#include <variant>

#include "ir_frwd.h"

template<typename T>
concept IsLocalValueType = std::is_base_of_v<ValueInstruction, T> ||
    std::is_same_v<T, ArgumentValue>;

class LocalValue final {
    explicit LocalValue(ArgumentValue* value) noexcept;
    explicit LocalValue(ValueInstruction * value) noexcept;

public:
    template <IsLocalValueType T>
    [[nodiscard]] T get() const {
        return std::get<T>(m_value);
    }

    template <IsLocalValueType T>
    [[nodiscard]]
    bool is() const {
        return std::holds_alternative<T>(m_value);
    }

    [[nodiscard]]
    const Type* type() const noexcept {
        return m_type;
    }

    void add_user(Instruction* user);

    friend std::ostream& operator<<(std::ostream& os, const LocalValue& obj);
    static std::expected<LocalValue, Error> from(const Value& value);

private:
    std::variant<
        ArgumentValue *,
        ValueInstruction *> m_value;
    const Type *m_type;
};

std::ostream& operator<<(std::ostream& os, const LocalValue& obj);