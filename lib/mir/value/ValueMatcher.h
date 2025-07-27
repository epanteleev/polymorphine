#pragma once

#include "Value.h"
#include "mir/types/TypeMatcher.h"

namespace impls {
    inline bool constant(const Value& value) {
        return value.is<double>() || value.is<std::int64_t>() || value.is<std::uint64_t>();
    }

    inline bool signed_v(const Value& value) {
        return signed_type(value.type());
    }

    inline bool unsigned_v(const Value& value) {
        return unsigned_type(value.type());
    }

    template<typename LHS, typename RHL>
    bool icmp(const Value& inst, LHS& l, RHL&& r) {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }
        const auto val = inst.get<ValueInstruction*>();
        if (const auto *icmp = dynamic_cast<const IcmpInstruction*>(val)) {
            return l(icmp->lhs()) && r(icmp->rhs());
        }

        return false;
    }

    inline bool alloc(const Value& inst) noexcept {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }

        const auto val = inst.get<ValueInstruction*>();
        return dynamic_cast<const Alloc*>(val) != nullptr;
    }

}

using ValueMatcher = bool(*)(const Value&);

consteval ValueMatcher constant() {
    return impls::constant;
}

consteval ValueMatcher signed_v() {
    return impls::signed_v;
}

consteval ValueMatcher unsigned_v() {
    return impls::unsigned_v;
}

template<typename LHS, typename RHS>
consteval auto icmp(LHS&& l, RHS&& r) noexcept {
    return [=](const Value& inst) {
        return impls::icmp(inst, l, r);
    };
}

consteval auto alloc() noexcept {
    return impls::alloc;
}

consteval auto argument() noexcept {
    return [](const Value& inst) { return inst.is<const ArgumentValue*>(); };
}