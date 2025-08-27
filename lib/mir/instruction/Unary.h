#pragma once

#include "ValueInstruction.h"

enum class UnaryOp {
    Negate,
    LogicalNot,
    Trunk,
    SignExtend,
    ZeroExtend,
    Ptr2Int,
    Flag2Int,
    Int2Ptr,
    Int2Float,
    Float2Int,
    Load
};

class Unary final: public ValueInstruction {
public:
    Unary(const PrimitiveType* type, const UnaryOp op, const Value& operand) noexcept:
        ValueInstruction(type, {operand}),
        m_op(op) {}

    [[nodiscard]]
    Value operand() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    UnaryOp op() const noexcept { return m_op; }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<Unary> load(const PrimitiveType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Load, value);
    }

    static std::unique_ptr<Unary> flag2int(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Flag2Int, value);
    }

    static std::unique_ptr<Unary> sext(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::SignExtend, value);
    }

    static std::unique_ptr<Unary> zext(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::ZeroExtend, value);
    }

private:
    const UnaryOp m_op;
};