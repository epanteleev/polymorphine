#pragma once

#include "LIRDef.h"
#include "LIRInstructionBase.h"
#include "LIRUse.h"


class LIRProducerInstructionBase : public LIRInstructionBase, public LIRUse, public LIRDef {
public:
    explicit LIRProducerInstructionBase(std::vector<LIROperand> &&uses) noexcept:
        LIRInstructionBase(std::move(uses)) {}
};