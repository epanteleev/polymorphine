#pragma once

#include <memory>
#include "Compare.h"

enum class IcmpPredicate {
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge
};

[[nodiscard]]
inline std::string_view to_string(const IcmpPredicate op) noexcept {
    switch (op) {
        case IcmpPredicate::Eq: return "eq";
        case IcmpPredicate::Ne: return "ne";
        case IcmpPredicate::Gt: return "gt";
        case IcmpPredicate::Ge: return "ge";
        case IcmpPredicate::Lt: return "lt";
        case IcmpPredicate::Le: return "le";
        default: std::unreachable();
    }
}

class IcmpInstruction final: public Compare {
public:
    explicit IcmpInstruction(const IcmpPredicate pred, const Value& lhs, const Value& rhs) noexcept:
        Compare(lhs, rhs),
        m_pred(pred) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    IcmpPredicate predicate() const noexcept {
        return m_pred;
    }

    static std::unique_ptr<IcmpInstruction> icmp(const IcmpPredicate pred, const Value& lhs, const Value& rhs) {
        return std::make_unique<IcmpInstruction>(pred, lhs, rhs);
    }

private:
    const IcmpPredicate m_pred;
};

namespace impls {
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
}

template<typename LHS, typename RHS>
consteval auto icmp(LHS&& l, RHS&& r) noexcept {
    return [=](const Value& inst) {
        return impls::icmp(inst, l, r);
    };
}