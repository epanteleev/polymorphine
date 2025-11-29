#pragma once

#include <memory>
#include "ValueInstruction.h"

enum class UnaryOp {
    Negate,
    LogicalNot,
    Trunk,
    Bitcast,
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
        return m_values[0];
    }

    [[nodiscard]]
    UnaryOp op() const noexcept { return m_op; }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    static std::unique_ptr<Unary> load(const PrimitiveType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Load, value);
    }

    [[nodiscard]]
    static std::unique_ptr<Unary> flag2int(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Flag2Int, value);
    }

    [[nodiscard]]
    static std::unique_ptr<Unary> sext(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::SignExtend, value);
    }

    [[nodiscard]]
    static std::unique_ptr<Unary> zext(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::ZeroExtend, value);
    }

    [[nodiscard]]
    static std::unique_ptr<Unary> trunc(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Trunk, value);
    }

    [[nodiscard]]
    static std::unique_ptr<Unary> bitcast(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Bitcast, value);
    }

private:
    const UnaryOp m_op;
};