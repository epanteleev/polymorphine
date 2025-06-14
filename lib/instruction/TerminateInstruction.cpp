#include "TerminateInstruction.h"
#include "BasicBlock.h"

std::span<BasicBlock * const> TerminateInstruction::successors() const {
    return m_successors;
}
