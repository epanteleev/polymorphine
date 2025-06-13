#pragma once

#include "ValueInstruction.h"

enum class UnaryOp {
    Negate,
    LogicalNot,
    Trunk,
    SignExtend,
    ZeroExtend,
    Ptr2Int,
    Int2Ptr,
    Int2Float,
    Float2Int,
    Load
};

class Unary final: public ValueInstruction {
public:
    Unary(const std::size_t id, BasicBlock *bb, const UnaryOp op, const Value& operand)
        : ValueInstruction(id, bb, operand.type(), {operand}), m_op(op) {}

    [[nodiscard]]
    Value operand() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    UnaryOp op() const { return m_op; }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<Unary> load(const Value &value) {
        return [&](std::size_t id, BasicBlock* bb) {
            return std::make_unique<Unary>(id, bb, UnaryOp::Load, value);
        };
    }

private:
    const UnaryOp m_op;
};