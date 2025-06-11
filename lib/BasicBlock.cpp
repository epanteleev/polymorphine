#include "BasicBlock.h"

#include <iostream>

void BasicBlock::print(std::ostream &os) const {
    if (m_id == 0) {
        os << "entry:";
    } else {
        os << 'L' << m_id << ":" << std::endl;
    }
    os << std::endl;

    for (const auto &inst : m_instructions) {
        os << "  ";
        inst.print(os);
        os << '\n';
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

void BasicBlock::make_edges(Instruction *inst)  {
    const auto term = Terminator::from(inst);
    if (!term.has_value()) {
        return;
    }

    for (auto& succ: term.value().targets()) {
        succ->m_predecessors.push_back(succ);
    }
}