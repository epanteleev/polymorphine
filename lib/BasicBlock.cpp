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
