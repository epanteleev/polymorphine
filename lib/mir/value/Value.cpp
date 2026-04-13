#include <ostream>

#include "mir/value/Value.h"
#include "mir/types/Type.h"
#include "mir/types/FlagType.h"
#include "mir/instruction/ValueInstruction.h"
#include "mir/value/ArgumentValue.h"
#include "mir/module/BasicBlock.h"
#include "mir/types/PointerType.h"
#include "mir/global/GlobalValue.h"
#include "mir/value/VConstant.h"

static_assert(std::is_trivially_copyable_v<Value>, "assumed to be");
static_assert(!std::is_polymorphic_v<Value>, "assumed to be");

Value::Value(const ArgumentValue *value) noexcept:
    m_value(const_cast<ArgumentValue *>(value)),
    m_type(value->type()) {}

Value::Value(const ValueInstruction *value) noexcept:
    m_value(const_cast<ValueInstruction *>(value)),
    m_type(value->type()) {}

Value::Value(const GlobalValue *value) noexcept:
    m_value(const_cast<GlobalValue *>(value)),
    m_type(PointerType::ptr()) {}

[[nodiscard]]
static Value::Variants to_variant(const VConstant& value) {
    if (FlagType::cast(value.type()) != nullptr) {
        return value.get<bool>();
    }

    if (FloatingPointType::cast(value.type()) != nullptr) {
        return value.get<double>();
    }

    if (IntegerType::cast(value.type()) != nullptr) {
        return value.get<std::int64_t>();
    }

    std::unreachable();
}

Value::Value(const VConstant &cst) noexcept:
    m_value(to_variant(cst)),
    m_type(cst.type()) {}

bool operator==(const Value& b, const Value& a) noexcept {
    if (&b == &a) {
        return true;
    }

    return a.m_type == b.m_type && a.m_value == b.m_value;
}

std::ostream& operator<<(std::ostream& os, const Value& obj) {
    auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, double> || std::is_same_v<T, std::int64_t>) {
            os << val;

        } else if constexpr (std::is_same_v<T, bool>) {
            if (val) {
                os << "true";
            } else {
                os << "false";
            }

        } else if constexpr (std::is_same_v<T, ArgumentValue *> || std::is_same_v<T, GlobalValue*>) {
            os << *val;

        } else if constexpr (std::is_same_v<T, ValueInstruction*>) {
            os << '%' << val->owner()->id() << 'x' << val->id();

        } else if constexpr (std::is_same_v<T, std::monostate>) {
            os << "undef";

        } else {
            static_assert(false, "Unsupported type in Value variant");
        }
    };

    std::visit(visitor, obj.m_value);
    return os;
}