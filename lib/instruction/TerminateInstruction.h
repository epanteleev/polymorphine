#pragma once

#include "Instruction.h"

enum class TermInstType {
    Branch,
    ConditionalBranch,
    Switch,
    Return,
    ReturnValue,
    VCall,
    IVCall
};

class TerminateInstruction final: public Instruction {
public:
    TerminateInstruction(const std::size_t id, BasicBlock *bb, const TermInstType type,
                         std::vector<BasicBlock *> &&successors)
        : Instruction(id, bb, {}), m_type(type), m_successors(std::move(successors)) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<TerminateInstruction> ret() {
        return [&] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<TerminateInstruction>(id, bb, TermInstType::Return, std::vector<BasicBlock *>{});
        };
    }

private:
    const TermInstType m_type;
    std::vector<BasicBlock* > m_successors;
};
