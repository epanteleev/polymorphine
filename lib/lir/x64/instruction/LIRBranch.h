#pragma once

#include "lir/x64/instruction/LIRControlInstruction.h"

class LIRBranch final: public LIRControlInstruction {
public:
    explicit LIRBranch(std::vector<LIROperand>&& uses,
                       std::vector<LIRBlock* >&& successors) :
        LIRControlInstruction(std::move(uses), std::move(successors)) {}

    void visit(LIRVisitor &visitor) override {
        visitor.jmp(succ(0));
    }

    static std::unique_ptr<LIRBranch> jmp(LIRBlock *target) {
        return std::make_unique<LIRBranch>(std::vector<LIROperand>{}, std::vector{target});
    }
};
