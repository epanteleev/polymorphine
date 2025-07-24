#pragma once

#include "LIRInstructionBase.h"


enum class LIRInstKind: std::uint8_t {
    Mov,
};

class LIRInstruction final: public LIRInstructionBase {
public:
    LIRInstruction(const std::size_t id, LIRBlock *bb, const LIRInstKind kind, std::vector<LIROperand>&& uses) :
        LIRInstructionBase(id, bb, std::move(uses)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

private:
    LIRInstKind m_kind;
};
