#include "TerminateValueInstruction.h"

TerminateValueInstruction::TerminateValueInstruction(NonTrivialType *ty,
                                                     const TermValueInstType type,
                                                     std::vector<BasicBlock *> &&successors)
    : ValueInstruction(ty, {}),
    m_type(type),
    m_successors(std::move(successors)) {}

std::span<BasicBlock *const> TerminateValueInstruction::successors() {
    return m_successors;
}