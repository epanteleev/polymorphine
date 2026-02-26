#include "LIRBlock.h"

#include "utility/Error.h"
#include "base/Constrains.h"
#include "lir/x64/instruction/LIRInstructionBase.h"

static_assert(CodeBlock<LIRBlock>, "assumed to be");

const LIRControlInstruction *LIRBlock::last() const {
    assertion(!m_instructions.empty(), "must be non empty");
    auto& inst = m_instructions.back();
    const auto term = dynamic_cast<LIRControlInstruction *>(&inst);
    assertion(term != nullptr, "must be terminate inst");
    return term;
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