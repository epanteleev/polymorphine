#pragma once

#include "lir/x64/instruction/LIRControlInstruction.h"

class LIRCondBranch final: public LIRControlInstruction {
public:
    explicit LIRCondBranch(const aasm::CondType kind, std::vector<LIROperand>&& uses,
                       std::vector<LIRBlock* >&& successors) noexcept:
        LIRControlInstruction(std::move(uses), std::move(successors)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override {
        visitor.jcc(m_kind, succ(0), succ(1));
    }

    static std::unique_ptr<LIRCondBranch> jcc(const aasm::CondType kind, LIRBlock *on_true, LIRBlock *on_false) {
        return std::make_unique<LIRCondBranch>(kind, std::vector<LIROperand>{}, std::vector{on_true, on_false});
    }

private:
    const aasm::CondType m_kind;
};
