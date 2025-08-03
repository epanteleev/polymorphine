#pragma once

#include <expected>
#include <iosfwd>
#include <span>
#include <variant>

#include "mir/mir_frwd.h"

template<typename T>
concept IsLocalValueType = std::derived_from<T, ValueInstruction> ||
    std::derived_from<T, ArgumentValue>;

class LocalValue final {
    explicit LocalValue(ArgumentValue* value) noexcept;
    explicit LocalValue(ValueInstruction * value) noexcept;

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

    [[nodiscard]]
    const Type* type() const noexcept {
        return m_type;
    }

    bool operator==(const LocalValue& other) const noexcept;

    void add_user(Instruction* user);

    [[nodiscard]]
    std::span<const Instruction* const> users() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LocalValue& obj);
    static std::expected<LocalValue, Error> try_from(const Value& value);

    template <IsLocalValueType T>
    static LocalValue from(const T* val) noexcept {
        return LocalValue(const_cast<T*>(val));
    }

private:
    std::variant<
        ArgumentValue *,
        ValueInstruction *> m_value;
    const Type *m_type;
};

std::ostream& operator<<(std::ostream& os, const LocalValue& obj);