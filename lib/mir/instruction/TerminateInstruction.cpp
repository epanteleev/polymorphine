#include "TerminateInstruction.h"
#include "mir/module/BasicBlock.h"

std::span<BasicBlock * const> TerminateInstruction::successors() const noexcept {
    return m_successors;
}
