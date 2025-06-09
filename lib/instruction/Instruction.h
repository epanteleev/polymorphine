#pragma once

#include <cstddef>
#include <functional>
#include <vector>

#include "../InstructionVisitor.h"
#include "../ir_frwd.h"
#include "../value/Value.h"

#include <iosfwd>
#include <memory>

#include "../BasicBlock.h"

template <typename T>
using InstructionBuilder = std::function<std::unique_ptr<T>(std::size_t, BasicBlock*)>;

class Instruction {
public:
    virtual ~Instruction() = default;

    Instruction(const std::size_t id, BasicBlock *bb, const std::initializer_list<Value>& values)
        : m_owner(bb), m_id(id), m_values(values) {}

    [[nodiscard]]
    std::size_t id() const { return m_id; }

    virtual void visit(Visitor& visitor) = 0;

    void print(std::ostream& os) const;

protected:
    BasicBlock* m_owner;
    const std::size_t m_id;
    std::vector<Value> m_values;
};

class ValueInstruction : public Instruction {
public:
    ValueInstruction(const std::size_t id, BasicBlock *bb, Type* ty, const std::initializer_list<Value>& values):
        Instruction(id, bb, values),
        m_ty(ty) {}

    [[nodiscard]]
    Type* type() const { return m_ty; }

protected:
    Type* m_ty;
    std::vector<Instruction*> m_users;
};

struct PhiEntry final {
    Value value;
    BasicBlock* predecessor{};
};

class PhiInstruction final: public ValueInstruction {
public:
    PhiInstruction(std::size_t id, BasicBlock *bb, NonTrivialType* ty, const std::initializer_list<Value>& values, std::vector<BasicBlock*> targets);

    void visit(Visitor &visitor) override { visitor.accept(this); }

private:
    std::vector<BasicBlock*> m_entries;
};

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

class Binary final: public ValueInstruction {
public:
    Binary(const std::size_t id, BasicBlock *bb, const BinaryOp op, const Value& lhs, const Value& rhs):
        ValueInstruction(id, bb, lhs.type(), {lhs, rhs}), m_op(op) {}

    [[nodiscard]]
    Value lhs() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    Value rhs() const {
        return m_values.at(1);
    }

    [[nodiscard]]
    BinaryOp op() const { return m_op; }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<Binary> add(const Value& lhs, const Value& rhs) {
        return [&] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<Binary>(id, bb, BinaryOp::Add, lhs, rhs);
        };
    }

    static InstructionBuilder<Binary> sub(const Value& lhs, const Value& rhs) {
        return [&] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<Binary>(id, bb, BinaryOp::Subtract, lhs, rhs);
        };
    }

private:
    const BinaryOp m_op;
};

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

private:
    const UnaryOp m_op;
};

enum class TermInstType {
    Branch,
    ConditionalBranch,
    Switch,
    Return,
    Call,
    VCall,
    ICall,
    IVCall
};

class TerminateInstruction final: public Instruction {
public:
    TerminateInstruction(const std::size_t id, BasicBlock *bb, const TermInstType type,
                         std::vector<BasicBlock *> &&successors)
        : Instruction(id, bb, {}), m_type(type), m_successors(std::move(successors)) {
    }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<TerminateInstruction> ret() {
        return [&] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<TerminateInstruction>(id, bb, TermInstType::Return, std::vector<BasicBlock *>{});
        };
    }
private:
    const TermInstType m_type;
    std::vector<BasicBlock* > m_successors;
};