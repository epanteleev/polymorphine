#include "Value.h"
#include "types/Type.h"

#include <iostream>
#include "instruction/Instruction.h"
#include "instruction/ValueInstruction.h"
#include "ArgumentValue.h"

Value::Value(ArgumentValue *value) noexcept : m_value(value), m_type(value->type()) {}

Value::Value(ValueInstruction *value) noexcept: m_value(value), m_type(value->type()) {}

std::ostream& operator<<(std::ostream& os, const Value& obj) {
    auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, double> || std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>) {
            os << val;

        } else if (std::is_same_v<T, ArgumentValue *>) {
            val->print(os);

        } else if constexpr (std::is_same_v<T, ValueInstruction*>) {
            os << '%' << val->id();

        } else {
            static_assert(IsValueType<T>, "Unsupported type in Value variant");
        }
    };

    std::visit(visitor, obj.m_value);
    return os;
}
