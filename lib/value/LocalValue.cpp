#include "LocalValue.h"

#include "instruction/ValueInstruction.h"
#include "ArgumentValue.h"
#include "value/Value.h"

LocalValue::LocalValue(ArgumentValue *value) noexcept :
    m_value(value),
    m_type(value->type()) {}

LocalValue::LocalValue(ValueInstruction *value) noexcept:
    m_value(value),
    m_type(value->type()) {}

std::expected<LocalValue, Error> LocalValue::from(const Value &value) {
    const auto visit = [&]<typename T>(const T &val) -> std::expected<LocalValue, Error> {
        if constexpr (std::is_same_v<T, ArgumentValue *> || std::is_same_v<T, ValueInstruction*>) {
            return LocalValue(val);
        } else {
            return std::unexpected(Error::CastError);
        }
    };

    return value.visit<std::expected<LocalValue, Error>>(visit);
}

void LocalValue::add_user(Instruction* user) {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, ArgumentValue *> || std::is_same_v<T, ValueInstruction*>) {
            val->add_user(user);
        } else {
            static_assert(false, "Somthing was wrong");
        }
    };

    std::visit(visitor, m_value);
}


std::ostream& operator<<(std::ostream& os, const LocalValue& obj) {
    auto visitor = [&]<typename T>(const T &val) {
        if (std::is_same_v<T, ArgumentValue *>) {
            val->print(os);

        } else if constexpr (std::is_same_v<T, ValueInstruction*>) {
            os << '%' << val->id();

        } else {
            static_assert(IsLocalValueType<T>, "Unsupported type in Value variant");
        }
    };

    std::visit(visitor, obj.m_value);
    return os;
}