#pragma once

#include "ValueInstruction.h"

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
    Compare(const std::size_t id, BasicBlock* bb, const Value& lhs, const Value& rhs):
        ValueInstruction(id, bb, FlagType::flag(),{lhs, rhs}) {}

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
    IcmpInstruction (const std::size_t id, BasicBlock* bb, IcmpPredicate pred, const Value& lhs, const Value& rhs):
        Compare(id, bb, lhs, rhs), m_pred(pred) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    IcmpPredicate predicate() const noexcept {
        return m_pred;
    }

    static InstructionBuilder<IcmpInstruction> icmp(IcmpPredicate pred, const Value& lhs, const Value& rhs) {
        return [=](std::size_t id, BasicBlock *bb) {
            return std::make_unique<IcmpInstruction>(id, bb, pred, lhs, rhs);
        };
    }

private:
    const IcmpPredicate m_pred;
};
