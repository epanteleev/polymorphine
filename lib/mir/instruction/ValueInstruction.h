#pragma once

#include "Instruction.h"

class ValueInstruction : public Instruction {
public:
    ValueInstruction(const Type* ty, const std::initializer_list<Value>& values) noexcept:
        Instruction(values),
        m_ty(ty) {}

    [[nodiscard]]
    const Type* type() const { return m_ty; }

    void add_user(const Instruction* user);

protected:
    const Type* m_ty;
    std::vector<const Instruction*> m_users;
};

class PhiInstruction final: public ValueInstruction {
public:
    PhiInstruction(NonTrivialType* ty, const std::initializer_list<Value>& values, std::vector<BasicBlock*> targets);

    void visit(Visitor &visitor) override { visitor.accept(this); }

private:
    std::vector<BasicBlock*> m_entries;
};
