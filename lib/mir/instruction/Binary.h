#pragma once

#include "ValueInstruction.h"

enum class BinaryOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    ShiftLeft,
    ShiftRight
};

class Binary final : public ValueInstruction {
public:
    Binary(const BinaryOp op, const Value &lhs, const Value &rhs) noexcept:
        ValueInstruction(lhs.type(), {lhs, rhs}),
        m_op(op) {}

    [[nodiscard]]
    const Value& lhs() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    const Value& rhs() const {
        return m_values.at(1);
    }

    [[nodiscard]]
    BinaryOp op() const { return m_op; }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<Binary> add(const Value &lhs, const Value &rhs) {
        return std::make_unique<Binary>(BinaryOp::Add, lhs, rhs);
    }

    static std::unique_ptr<Binary> sub(const Value &lhs, const Value &rhs) {
        return std::make_unique<Binary>(BinaryOp::Subtract, lhs, rhs);
    }

private:
    const BinaryOp m_op;
};
