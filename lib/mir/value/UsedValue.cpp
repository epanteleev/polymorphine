#include "UsedValue.h"

#include "mir/instruction/ValueInstruction.h"
#include "mir/global/GlobalValue.h"
#include "mir/value/ArgumentValue.h"
#include "mir/value/Value.h"

#include "utility/Error.h"

static_assert(IsLocalValueType<ArgumentValue>, "sanity check");
static_assert(IsLocalValueType<ValueInstruction>, "sanity check");

UsedValue::UsedValue(ArgumentValue *value) noexcept :
    m_value(value) {}

UsedValue::UsedValue(ValueInstruction *value) noexcept:
    m_value(value) {}

std::expected<UsedValue, Error> UsedValue::try_from(const Value &value) {
    const auto visit = []<typename T>(const T &val) -> std::expected<UsedValue, Error> {
        if constexpr (IsLocalValueType<std::remove_pointer_t<T>>) {
            return UsedValue(val);
        } else {
            return std::unexpected(Error::CastError);
        }
    };

    return value.visit(visit);
}

bool UsedValue::operator==(const UsedValue &other) const noexcept {
    return m_value == other.m_value;
}

void UsedValue::add_user(Instruction* user) {
    const auto visitor = [&]<typename T>(const T &val) {
        val->add_user(user);
    };

    std::visit(visitor, m_value);
}

std::span<const Instruction * const> UsedValue::users() const noexcept {
    const auto visitor = []<typename T>(const T &val) {
        return val->users();
    };

    return std::visit(visitor, m_value);
}