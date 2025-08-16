
#include "mir/module/BasicBlock.h"
#include "mir/instruction/Terminator.h"
#include "utility/Error.h"
#include "mir/value/LocalValue.h"

Terminator BasicBlock::last() const noexcept {
    const auto back = m_instructions.back();
    if (!back) {
        die("BasicBlock has no instructions");
    }

    const auto term = Terminator::from(back.value());
    if (!term.has_value()) {
        die("Last instruction is not a terminator");
    }

    return term.value();
}

void BasicBlock::make_def_use_chain(Instruction *inst) {
    for (const auto& operand: inst->operands()) {
        auto local = LocalValue::try_from(operand);
        if (!local.has_value()) {
            continue;
        }

        local->add_user(inst);
    }
}