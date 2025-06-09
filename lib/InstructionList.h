#pragma once

#include "Instruction.h"
#include "utlility/OrderedSet.h"


class InstructionList final {
public:
    Instruction* push_back(UnaryOp op, Value operand) {
        return m_instructions.push_back<UnaryInstruction>(op, operand);
    }


private:
    OrderedSet<Instruction> m_instructions;
};
