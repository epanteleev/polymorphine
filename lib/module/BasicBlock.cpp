#include <iostream>

#include "module/BasicBlock.h"
#include "instruction/Terminator.h"
#include "utility/Error.h"
#include "value/LocalValue.h"

Terminator BasicBlock::last() const {
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

void BasicBlock::print(std::ostream &os) const {
    print_short_name(os);
    os << ':' << std::endl;

    for (const auto &inst : m_instructions) {
        os << "  ";
        inst.print(os);
        os << '\n';
    }
}

void BasicBlock::print_short_name(std::ostream &os) const {
    if (m_id == 0) {
        os << "entry";
    } else {
        os << 'L' << m_id;
    }
}

void BasicBlock::make_def_use_chain(Instruction *inst) {
    for (const auto& operand: inst->operands()) {
        auto local = LocalValue::from(operand);
        if (!local.has_value()) {
            continue;
        }

        local.value().add_user(inst);
    }
}