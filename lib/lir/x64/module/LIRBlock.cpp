#include "LIRBlock.h"

#include "utility/Error.h"
#include "lir/x64/instruction/LIRInstructionBase.h"

const LIRControlInstruction *LIRBlock::last() const {
    assertion(!m_instructions.empty(), "must be non empty");
    auto& inst = m_instructions.back();
    return dynamic_cast<LIRControlInstruction *>(&inst);
}

void LIRBlock::make_def_use_chain(LIRInstructionBase *inst) {
    for (const auto &in: inst->inputs()) {
        auto local = LIRVal::try_from(in);
        if (!local.has_value()) {
            continue;
        }

        local->add_user(inst);
    }
}