#pragma once

#include <cstddef>
#include <vector>

#include "ir_frwd.h"
#include "value/Value.h"

class Instruction {
public:
    Instruction(const std::size_t id, BasicBlock *bb)
        : m_owner(bb), m_id(id) {}

    [[nodiscard]]
    std::size_t id() const { return m_id; }

protected:
    BasicBlock* m_owner;
    const std::size_t m_id;
    std::vector<Value> m_values;
};

class ValueInstruction : public Instruction {
public:
    ValueInstruction(const std::size_t id, BasicBlock *bb): Instruction(id, bb) {}

protected:
    std::vector<Instruction*> m_users;
};

struct PhiEntry final {
    Value value;
    BasicBlock* predecessor{};
};

class PhiInstruction : public ValueInstruction {
public:
    PhiInstruction(const std::size_t id, BasicBlock *bb, std::vector<PhiEntry> &&entries)
        : ValueInstruction(id, bb), m_entries(std::move(entries)) {}

private:
    std::vector<PhiEntry> m_entries;
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

class BinaryInstruction final: public Instruction {
public:
    BinaryInstruction(const std::size_t id, BasicBlock *bb, const BinaryOp op): Instruction(id, bb), m_op(op) {}

    [[nodiscard]]
    Value lhs() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    Value rhs() const {
        return m_values.at(1);
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

class UnaryInstruction final: public Instruction {
public:
    UnaryInstruction(BasicBlock *bb, const std::size_t id, const UnaryOp op)
        : Instruction(id, bb), m_op(op) {}

    [[nodiscard]]
    Value operand() const {
        return m_values.at(0);
    }

private:
    const UnaryOp m_op;
};

enum class TermInstType {
    Branch,
    ConditionalBranch,
    Return,
    Call,
    VCall,
    ICall,
    IVCall
};

class TerminateInstruction final: public Instruction {
public:
    TerminateInstruction(BasicBlock *bb, const std::size_t id, const TermInstType type,
                         std::vector<BasicBlock *> &&successors)
        : Instruction(id, bb), m_type(type), m_successors(std::move(successors)) {
    }

private:
    const TermInstType m_type;
    std::vector<BasicBlock* > m_successors;
};