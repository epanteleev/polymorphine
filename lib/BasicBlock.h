#pragma once

#include <vector>
#include "Instruction.h"
#include "utility/OrderedSet.h"

class BasicBlock final {
public:
    UnaryInstruction* push_back(const UnaryOp op, const Value& operand) {
        return m_instructions.push_back<UnaryInstruction>(this, op, operand);
    }

    BinaryInstruction* push_back(const BinaryOp op, const Value& lhs, const Value& rhs) {
        return m_instructions.push_back<BinaryInstruction>(this, op, lhs, rhs);
    }

    TerminateInstruction* push_back(const TermInstType type,
                                                  std::vector<BasicBlock *> &&successors) {
        return m_instructions.push_back<TerminateInstruction>(this, type, std::move(successors));
    }

private:
    std::vector<BasicBlock *> m_predecessors;
    std::vector<BasicBlock *> m_successors;
    OrderedSet<Instruction> m_instructions;
};
