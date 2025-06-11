#include "TerminateInstruction.h"
#include "BasicBlock.h"

std::span<BasicBlock *> TerminateInstruction::successors() {
    return m_successors;
}