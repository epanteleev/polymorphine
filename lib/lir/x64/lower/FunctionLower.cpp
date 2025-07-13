#include "FunctionLower.h"

#include "mir/types/TypeMatchingRules.h"

template<std::integral T>
static LirCst make_constant(const Type& type, const T integer) noexcept {
    if (type.isa(i8()) || type.isa(u8())) {
        return LirCst::imm8(static_cast<std::int8_t>(integer));
    }

    if (type.isa(i16()) || type.isa(u16())) {
        return LirCst::imm16(static_cast<std::int16_t>(integer));
    }

    if (type.isa(i32()) || type.isa(u32())) {
        return LirCst::imm32(static_cast<std::int32_t>(integer));
    }

    if (type.isa(i64()) || type.isa(u64())) {
        return LirCst::imm64(static_cast<std::int64_t>(integer));
    }

    die("Unsupported type for constant");
}

LIROperand FunctionLower::get_mapping(const Value &val) {
    const auto visitor = [&]<typename T>(const T &v) -> LIROperand {
        if constexpr (std::is_same_v<T, double>) {
            unimplemented();

        } else if constexpr (std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>) {
            return make_constant(*val.type(), v);

        } else if constexpr (std::is_same_v<T, const ArgumentValue *> || std::is_same_v<T, ValueInstruction *>) {
            const auto local = LocalValue::from(v);
            return m_mapping.at(local);

        } else {
            static_assert(false, "Unsupported type in Value variant");
            std::unreachable();
        }
    };

    return val.visit<LIROperand>(visitor);
}
