#pragma once

#include "Value.h"
#include "mir/types/TypeMatcher.h"

namespace impls {
    inline bool any_value(const Value&) noexcept {
        return true;
    }

    inline bool constant(const Value& value) noexcept {
        return value.is<double>() || value.is<std::int64_t>();
    }

    inline bool signed_v(const Value& value) noexcept {
        return signed_type(value.type());
    }

    inline bool unsigned_v(const Value& value) noexcept {
        return unsigned_type(value.type());
    }

    template<typename LHS, typename RHL>
    bool icmp(const Value& inst, LHS& l, RHL&& r) noexcept {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }
        const auto val = inst.get<ValueInstruction*>();
        if (const auto *icmp = dynamic_cast<const IcmpInstruction*>(val)) {
            return l(icmp->lhs()) && r(icmp->rhs());
        }

        return false;
    }

    template<typename T>
    bool value_inst(const Value& value) noexcept {
        if (!value.is<ValueInstruction*>()) {
            return false;
        }

        const auto value_instruction = value.get<ValueInstruction*>();
        return dynamic_cast<const T*>(value_instruction) != nullptr;
    }

    inline bool any_stack_alloc(const Value& value) noexcept {
        if (value.is<ArgumentValue*>()) {
            if (const auto arg = value.get<ArgumentValue*>(); arg->attributes().has(Attribute::ByValue)) {
                return true;
            }
        }

        return value_inst<Alloc>(value);
    }

    template<typename... Args>
    bool match_args(const std::span<const Value>& values, Args&& ...args) noexcept {
        std::size_t index{};
        const auto matcher = [&]<typename T>(T&& arg) {
            if (!arg(values[index])) {
                return false;
            }
            index++;
            return true;
        };

        return (matcher(std::forward<Args>(args)) && ...);
    }

    template<typename T, typename ...Args>
    bool value_inst_with_operands(const Value& inst, Args&& ...args) noexcept {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }
        const auto val = inst.get<ValueInstruction*>();
        if (const auto *typed = dynamic_cast<const T*>(val)) {
            return match_args(typed->operands(), std::forward<Args>(args)...);
        }

        return false;
    }

    inline bool integral(const Value& value, const std::uint64_t cst) noexcept {
        if (value.is<std::int64_t>()) {
            return value.get<std::int64_t>() == static_cast<std::int64_t>(cst);
        }

        return false;
    }
}

consteval auto any_value() noexcept {
    return impls::any_value;
}

template<typename Matcher>
consteval auto neg(Matcher&& matcher) noexcept {
    return [=](const Value& value) {
        return !matcher(value);
    };
}

consteval auto constant() {
    return impls::constant;
}

consteval auto signed_v() {
    return impls::signed_v;
}

consteval auto unsigned_v() {
    return impls::unsigned_v;
}

template<typename LHS, typename RHS>
consteval auto icmp(LHS&& l, RHS&& r) noexcept {
    return [=](const Value& inst) {
        return impls::icmp(inst, l, r);
    };
}

consteval auto alloc() noexcept {
    return impls::value_inst<Alloc>;
}

consteval auto any_stack_alloc() noexcept {
    return impls::any_stack_alloc;
}

consteval auto field_access() noexcept {
    return impls::value_inst<FieldAccess>;
}

consteval auto gep() noexcept {
    return impls::value_inst<GetElementPtr>;
}

template<typename Matcher1, typename Matcher2>
consteval auto gep(Matcher1&& pointer, Matcher2&& index) noexcept {
    return [=](const Value& inst) {
        return impls::value_inst_with_operands<GetElementPtr>(inst, pointer, index);
    };
}

consteval auto gfp() noexcept {
    return impls::value_inst<GetFieldPtr>;
}

consteval auto argument() noexcept {
    return [](const Value& inst) { return inst.is<ArgumentValue*>(); };
}

consteval auto integral(const std::uint64_t cst) noexcept {
    return [=](const Value& value) { return impls::integral(value, cst); };
}