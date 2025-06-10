#pragma once

#include "ValueInstruction.h"

enum class TermValueInstType {
    Call,
    ICall,
};

class TerminateValueInstruction final : public ValueInstruction {
public:
    TerminateValueInstruction(std::size_t id, BasicBlock *bb, NonTrivialType* ty, TermValueInstType type,
                         std::vector<BasicBlock *> &&successors);

    void visit(Visitor &visitor) override { visitor.accept(this); }

private:
    const TermValueInstType m_type;
    std::vector<BasicBlock* > m_successors;
};
