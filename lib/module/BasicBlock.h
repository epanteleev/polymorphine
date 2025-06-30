#pragma once

#include "base/BasicBlockBase.h"
#include "pass/Constrains.h"
#include "instruction/Instruction.h"
#include "instruction/Terminator.h"


class BasicBlock final: public BasicBlockBase<BasicBlock, Instruction> {
public:
    explicit BasicBlock(const std::size_t id): BasicBlockBase(id) {}

    template<std::derived_from<Instruction> U>
    U* push_back(const InstructionBuilder<U>& fn) {
        auto creator = [&] (std::size_t id) {
            return fn(id, this);
        };

        auto inst = m_instructions.push_back<U>(creator);
        make_def_use_chain(inst);
        if constexpr (IsTerminator<U>) {
            make_edges(inst);
        }

        return inst;
    }

    [[nodiscard]]
    Terminator last() const;

    [[nodiscard]]
    std::span<BasicBlock* const> successors() const {
        return last().targets();
    }

private:
    static void make_def_use_chain(Instruction* inst);

    template<IsTerminator T>
    static void make_edges(T *inst)  {
        for (auto block: inst->successors()) {
            block->m_predecessors.push_back(inst->owner());
        }
    }
};


static_assert(CodeBlock<BasicBlock>, "assumed to be");