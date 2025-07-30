#include "mir/value/Value.h"
#include "mir/types/Type.h"

#include <ostream>
#include "mir/instruction/Instruction.h"
#include "mir/instruction/ValueInstruction.h"
#include "ArgumentValue.h"

Value::Value(const ArgumentValue *value) noexcept : m_value(value), m_type(value->type()) {}

Value::Value(const ValueInstruction *value) noexcept: m_value(value), m_type(value->type()) {}

std::ostream& operator<<(std::ostream& os, const Value& obj) {
    auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, double> ||
            std::is_same_v<T, std::int64_t> ||
            std::is_same_v<T, std::uint64_t> ||
            std::is_same_v<T, const ArgumentValue *>) {
            os << val;

        } else if constexpr (std::is_same_v<T, const ValueInstruction*>) {
            os << '%' << val->id();

        } else {
            static_assert(false, "Unsupported type in Value variant");
        }
    };

    std::visit(visitor, obj.m_value);
    return os;
}
