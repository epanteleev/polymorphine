#pragma once

#include "Value.h"
#include "mir/types/TypeMatcher.h"

namespace impls {
    static bool any_value(const Value&) noexcept {
        return true;
    }

    static bool constant(const Value& value) noexcept {
        return value.is<double>() || value.is<std::int64_t>();
    }

    static bool signed_v(const Value& value) noexcept {
        return signed_type(value.type());
    }

    static bool unsigned_v(const Value& value) noexcept {
        return unsigned_type(value.type());
    }

    template<typename LHS, typename RHL>
    static bool icmp(const Value& inst, LHS& l, RHL&& r) noexcept {
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
    static bool value_inst(const Value& inst) noexcept {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }

        const auto val = inst.get<ValueInstruction*>();
        return dynamic_cast<const T*>(val) != nullptr;
    }

    template<typename... Args>
    static bool match_args(const std::span<const Value>& values, Args&& ...args) noexcept {
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
    static bool value_inst_with_operands(const Value& inst, Args&& ...args) noexcept {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }
        const auto val = inst.get<ValueInstruction*>();
        if (const auto *typed = dynamic_cast<const T*>(val)) {
            return match_args(typed->operands(), std::forward<Args>(args)...);
        }

        return false;
    }

    static bool integral(const Value& value, const std::uint64_t cst) noexcept {
        if (value.is<std::int64_t>()) {
            return value.get<std::int64_t>() == static_cast<std::int64_t>(cst);
        }

        return false;
    }
}

static consteval auto any_value() noexcept {
    return impls::any_value;
}

template<typename Matcher>
static consteval auto neg(Matcher&& matcher) noexcept {
    return [=](const Value& value) {
        return !matcher(value);
    };
}

static consteval auto constant() {
    return impls::constant;
}

static consteval auto signed_v() {
    return impls::signed_v;
}

static consteval auto unsigned_v() {
    return impls::unsigned_v;
}

template<typename LHS, typename RHS>
static consteval auto icmp(LHS&& l, RHS&& r) noexcept {
    return [=](const Value& inst) {
        return impls::icmp(inst, l, r);
    };
}

static consteval auto alloc() noexcept {
    return impls::value_inst<Alloc>;
}

static consteval auto field_access() noexcept {
    return impls::value_inst<FieldAccess>;
}

static consteval auto gep() noexcept {
    return impls::value_inst<GetElementPtr>;
}

template<typename Matcher1, typename Matcher2>
static consteval auto gep(Matcher1&& pointer, Matcher2&& index) noexcept {
    return [=](const Value& inst) {
        return impls::value_inst_with_operands<GetElementPtr>(inst, pointer, index);
    };
}

static consteval auto gfp() noexcept {
    return impls::value_inst<GetFieldPtr>;
}

static consteval auto argument() noexcept {
    return [](const Value& inst) { return inst.is<ArgumentValue*>(); };
}

static consteval auto integral(const std::uint64_t cst) noexcept {
    return [=](const Value& value) { return impls::integral(value, cst); };
}