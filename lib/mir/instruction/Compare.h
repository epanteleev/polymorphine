#pragma once

#include "ValueInstruction.h"
#include "mir/types/FlagType.h"

enum class IcmpPredicate {
    Eq,
    Ne,
    Lt,
    Le,
    Gt,
    Ge
};

class Compare: public ValueInstruction {
public:
    Compare(const Value& lhs, const Value& rhs) noexcept:
        ValueInstruction(FlagType::flag(),{lhs, rhs}) {}

    [[nodiscard]]
    const Value& lhs() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    const Value& rhs() const {
        return m_values.at(1);
    }
};

class IcmpInstruction final: public Compare {
public:
    IcmpInstruction(const IcmpPredicate pred, const Value& lhs, const Value& rhs) noexcept:
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
