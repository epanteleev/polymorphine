#include "mir/module/BasicBlock.h"
#include "mir/instruction/Terminator.h"
#include "utility/Error.h"
#include "mir/value/UsedValue.h"

Terminator BasicBlock::last() const noexcept {
    const auto back = m_instructions.back();
    if (back == m_instructions.end()) {
        die("BasicBlock has no instructions");
    }

    const auto term = Terminator::from(back.get());
    if (!term.has_value()) {
        die("Last instruction is not a terminator");
    }

    return term.value();
}

void BasicBlock::make_def_use_chain(Instruction *inst) {
    for (const auto& operand: inst->operands()) {
        auto local = UsedValue::try_from(operand);
        if (!local.has_value()) {
            continue;
        }

        local->add_user(inst);
    }
}