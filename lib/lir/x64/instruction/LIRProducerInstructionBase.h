#pragma once

#include "LIRDef.h"
#include "LIRInstructionBase.h"
#include "LIRUse.h"


class LIRProducerInstructionBase: public LIRInstructionBase, public LIRUse, public LIRDef {
public:
    explicit LIRProducerInstructionBase(const InplaceVec<LIRValType, 2>& type, std::vector<LIROperand> &&uses) noexcept:
        LIRInstructionBase(std::move(uses)),
        LIRDef(type) {}
};