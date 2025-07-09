#include "TerminateValueInstruction.h"

std::span<BasicBlock *const> TerminateValueInstruction::successors() {
    return m_successors;
}