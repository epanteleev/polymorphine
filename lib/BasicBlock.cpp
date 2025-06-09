#include "BasicBlock.h"

#include <iostream>

void BasicBlock::print(std::ostream &os) const {
    os << 'L' << m_id << ":" << std::endl;
    for (const auto &inst : m_instructions) {
        os << "  ";
        inst.print(os);
        os << '\n';
    }
}
