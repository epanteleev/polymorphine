#pragma once

#include "ValueInstruction.h"

enum class BinaryOp: std::uint8_t {
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
        return m_values[0];
    }

    [[nodiscard]]
    const Value& rhs() const {
        return m_values[1];
    }

    [[nodiscard]]
    BinaryOp op() const { return m_op; }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    static std::unique_ptr<Binary> add(const Value &lhs, const Value &rhs) {
        return std::make_unique<Binary>(BinaryOp::Add, lhs, rhs);
    }

    [[nodiscard]]
    static std::unique_ptr<Binary> sub(const Value &lhs, const Value &rhs) {
        return std::make_unique<Binary>(BinaryOp::Subtract, lhs, rhs);
    }

    [[nodiscard]]
    static std::unique_ptr<Binary> shl(const Value &lhs, const Value &count) {
        return std::make_unique<Binary>(BinaryOp::ShiftLeft, lhs, count);
    }

    [[nodiscard]]
    static std::unique_ptr<Binary> shr(const Value &lhs, const Value &count) {
        return std::make_unique<Binary>(BinaryOp::ShiftRight, lhs, count);
    }

    [[nodiscard]]
    static std::unique_ptr<Binary> xxor(const Value &lhs, const Value &rhs) {
        return std::make_unique<Binary>(BinaryOp::BitwiseXor, lhs, rhs);
    }

    [[nodiscard]]
    static std::unique_ptr<Binary> div(const Value &lhs, const Value &rhs) {
        return std::make_unique<Binary>(BinaryOp::Divide, lhs, rhs);
    }

private:
    const BinaryOp m_op;
};
