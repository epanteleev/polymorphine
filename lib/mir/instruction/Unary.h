#pragma once

#include <memory>
#include "ValueInstruction.h"
#include "mir/types/PointerType.h"

enum class UnaryOp: std::uint8_t {
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
    const Value& operand() const {
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
    static std::unique_ptr<Unary> int2fp(const FloatingPointType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Int2Float, value);
    }

    [[nodiscard]]
    static std::unique_ptr<Unary> fp2int(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Float2Int, value);
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

    [[nodiscard]]
    static std::unique_ptr<Unary> ptr2int(const IntegerType* ty, const Value &value) {
        return std::make_unique<Unary>(ty, UnaryOp::Ptr2Int, value);
    }

    [[nodiscard]]
    static std::unique_ptr<Unary> int2ptr(const Value &value) {
        return std::make_unique<Unary>(PointerType::ptr(), UnaryOp::Int2Ptr, value);
    }

private:
    const UnaryOp m_op;
};