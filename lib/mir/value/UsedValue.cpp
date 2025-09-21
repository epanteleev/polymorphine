#include "UsedValue.h"

#include "mir/instruction/ValueInstruction.h"
#include "mir/global/GlobalVariable.h"
#include "mir/global/GlobalConstant.h"
#include "mir/types/PointerType.h"
#include "mir/value/ArgumentValue.h"
#include "mir/value/Value.h"

#include "utility/Error.h"

static_assert(IsLocalValueType<ArgumentValue>, "sanity check");
static_assert(IsLocalValueType<ValueInstruction>, "sanity check");
static_assert(IsLocalValueType<GlobalVariable>, "sanity check");

UsedValue::UsedValue(ArgumentValue *value) noexcept :
    m_value(value) {}

UsedValue::UsedValue(ValueInstruction *value) noexcept:
    m_value(value) {}

UsedValue::UsedValue(GlobalVariable *value) noexcept:
    m_value(value) {}

std::expected<UsedValue, Error> UsedValue::try_from(const Value &value) {
    const auto visit = [&]<typename T>(const T &val) -> std::expected<UsedValue, Error> {
        if constexpr (IsLocalValueType<std::remove_pointer_t<T>>) {
            return UsedValue(val);
        } else {
            return std::unexpected(Error::CastError);
        }
    };

    return value.visit(visit);
}

const Type * UsedValue::type() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        return val->type();
    };

    return std::visit(visitor, m_value);
}

bool UsedValue::operator==(const UsedValue &other) const noexcept {
    if (&other == this) {
        return true;
    }

    return m_value == other.m_value;
}

void UsedValue::add_user(Instruction* user) {
    const auto visitor = [&]<typename T>(const T &val) {
        val->add_user(user);
    };

    std::visit(visitor, m_value);
}

std::span<const Instruction * const> UsedValue::users() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        return val->users();
    };

    return std::visit(visitor, m_value);
}


std::ostream& operator<<(std::ostream& os, const UsedValue& obj) {
    auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, ArgumentValue *> || std::same_as<T, GlobalVariable*> ) {
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