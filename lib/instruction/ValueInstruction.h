#pragma once

#include "Instruction.h"

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

class PhiInstruction final: public ValueInstruction {
public:
    PhiInstruction(std::size_t id, BasicBlock *bb, NonTrivialType* ty, const std::initializer_list<Value>& values, std::vector<BasicBlock*> targets);

    void visit(Visitor &visitor) override { visitor.accept(this); }

private:
    std::vector<BasicBlock*> m_entries;
};
