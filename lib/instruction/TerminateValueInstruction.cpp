#include "TerminateValueInstruction.h"

std::span<BasicBlock *> TerminateValueInstruction::successors() {
    return m_successors;
}