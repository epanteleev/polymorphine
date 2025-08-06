#pragma once

#include "LIRInstructionBase.h"


enum class LIRInstKind: std::uint8_t {
    Mov,
    Store,
    Cmp,
};

class LIRInstruction final: public LIRInstructionBase {
public:
    LIRInstruction(const LIRInstKind kind, std::vector<LIROperand>&& uses) :
        LIRInstructionBase(std::move(uses)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRInstruction> mov(const LIRVal& dst, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Mov, std::vector<LIROperand>{dst, src});
    }

    static std::unique_ptr<LIRInstruction> store(const LIRVal& dst, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Store, std::vector<LIROperand>{dst, src});
    }

    static std::unique_ptr<LIRInstruction> cmp(const LIROperand &lhs, const LIROperand &rhs) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Cmp, std::vector{lhs, rhs});
    }

private:
    LIRInstKind m_kind;
};
