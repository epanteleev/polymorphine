#include "LocalValue.h"

#include "mir/instruction/ValueInstruction.h"
#include "ArgumentValue.h"
#include "mir/value/Value.h"
#include "utility/Error.h"

static_assert(IsLocalValueType<ArgumentValue>, "sanity check");
static_assert(IsLocalValueType<ValueInstruction>, "sanity check");

LocalValue::LocalValue(ArgumentValue *value) noexcept :
    m_value(value),
    m_type(value->type()) {}

LocalValue::LocalValue(ValueInstruction *value) noexcept:
    m_value(value),
    m_type(value->type()) {}

std::expected<LocalValue, Error> LocalValue::try_from(const Value &value) {
    const auto visit = [&]<typename T>(const T &val) -> std::expected<LocalValue, Error> {
        if constexpr (std::is_same_v<T, ArgumentValue *> || std::is_same_v<T, ValueInstruction *>) {
            return LocalValue(val);
        } else {
            return std::unexpected(Error::CastError);
        }
    };

    return value.visit<std::expected<LocalValue, Error>>(visit);
}

bool LocalValue::operator==(const LocalValue &other) const noexcept {
    if (&other == this) {
        return true;
    }

    return m_value == other.m_value && m_type == other.m_type;
}

void LocalValue::add_user(Instruction* user) {
    const auto visitor = [&]<typename T>(const T &val) {
        val->add_user(user);
    };

    std::visit(visitor, m_value);
}

std::span<const Instruction * const> LocalValue::users() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        return val->users();
    };

    return std::visit(visitor, m_value);
}


std::ostream& operator<<(std::ostream& os, const LocalValue& obj) {
    auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, ArgumentValue *>) {
            os << val;

        } else if constexpr (std::is_same_v<T, ValueInstruction*>) {
            os << '%' << val->id();

        } else {
            static_assert(false, "Unsupported type in Value variant");
        }
    };

    std::visit(visitor, obj.m_value);
    return os;
}
