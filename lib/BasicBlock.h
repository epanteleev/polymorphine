#pragma once

#include <vector>
#include "Instruction.h"

class BasicBlock final {
public:

private:
    std::vector<BasicBlock *> m_predecessors;
    std::vector<BasicBlock *> m_successors;
    std::vector<Instruction *> m_instructions;
};
