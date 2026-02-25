#include "mir/module/BasicBlock.h"
#include "mir/instruction/Terminator.h"
#include "utility/Error.h"
#include "mir/value/UsedValue.h"

Terminator BasicBlock::last() const noexcept {
    assertion(!m_instructions.empty(), "must be non empty");
    auto& last = m_instructions.back();

    const auto term = Terminator::from(&last);
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