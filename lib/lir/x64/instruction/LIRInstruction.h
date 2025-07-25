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

    static LIRInstBuilder<LIRInstruction> mov(const LIRVal& dst, const LIROperand& src) {
        return [=](std::size_t id, LIRBlock *bb) {
            return std::make_unique<LIRInstruction>(id, bb, LIRInstKind::Mov, std::vector<LIROperand>{dst, src});
        };
    }

private:
    LIRInstKind m_kind;
};
