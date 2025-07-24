#pragma once
#include "LIRInstruction.h"


class LIRProducerInstructionBase : public LIRInstructionBase {
public:
    LIRProducerInstructionBase(std::size_t id, LIRBlock *bb, std::vector<LIROperand> &&uses) noexcept:
        LIRInstructionBase(id, bb, std::move(uses)) {}
};
