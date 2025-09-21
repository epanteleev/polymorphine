#pragma once

#include <expected>
#include <iosfwd>
#include <span>
#include <variant>

#include "mir/mir_frwd.h"

template<typename T>
concept IsLocalValueType = std::derived_from<T, ValueInstruction> ||
    std::derived_from<T, ArgumentValue> ||
    std::derived_from<T, GlobalVariable>;

class UsedValue final {
    explicit UsedValue(ArgumentValue* value) noexcept;
    explicit UsedValue(ValueInstruction * value) noexcept;
    explicit UsedValue(GlobalVariable* value) noexcept;

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
    const Type* type() const noexcept;

    bool operator==(const UsedValue& other) const noexcept;

    void add_user(Instruction* user);

    [[nodiscard]]
    std::span<const Instruction* const> users() const noexcept;

    friend std::ostream& operator<<(std::ostream& os, const UsedValue& obj);
    static std::expected<UsedValue, Error> try_from(const Value& value);

    template <IsLocalValueType T>
    static UsedValue from(const T* val) noexcept {
        return UsedValue(const_cast<T*>(val));
    }

private:
    std::variant<
        ArgumentValue *,
        ValueInstruction *,
        GlobalVariable* > m_value;
};

std::ostream& operator<<(std::ostream& os, const UsedValue& obj);