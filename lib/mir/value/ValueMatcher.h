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

    template<typename T>
    bool value_inst(const Value& inst) noexcept {
        if (!inst.is<ValueInstruction*>()) {
            return false;
        }

        const auto val = inst.get<ValueInstruction*>();
        return dynamic_cast<const T*>(val) != nullptr;
    }

    inline bool integral(const Value& value, const std::uint64_t cst) noexcept {
        if (value.is<std::int64_t>()) {
            return value.get<std::int64_t>() == static_cast<std::int64_t>(cst);
        }
        if (value.is<std::uint64_t>()) {
            return value.get<std::uint64_t>() == cst;
        }
        return false;
    }
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

consteval auto gep() noexcept {
    return impls::value_inst<GetElementPtr>;
}

consteval auto argument() noexcept {
    return [](const Value& inst) { return inst.is<ArgumentValue*>(); };
}

consteval auto integral(const std::uint64_t cst) noexcept {
    return [=](const Value& value) { return impls::integral(value, cst); };
}