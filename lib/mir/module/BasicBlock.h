#pragma once

#include "base/BasicBlockBase.h"
#include "base/Constrains.h"
#include "mir/instruction/Instruction.h"
#include "mir/instruction/Terminator.h"


class BasicBlock final: public BasicBlockBase<BasicBlock, Instruction> {
public:
    template<std::derived_from<Instruction> U>
    U* ins(std::unique_ptr<U>&& inst) {
        auto inst_ptr = inst.get();
        const auto id = m_instructions.push_back(std::move(inst));
        inst_ptr->connect(id, this);
        make_def_use_chain(inst_ptr);
        if constexpr (IsTerminator<U>) {
            make_edges(inst_ptr);
        }

        return inst_ptr;
    }

    [[nodiscard]]
    Terminator last() const noexcept;

    [[nodiscard]]
    std::span<BasicBlock* const> successors() const {
        return last().targets();
    }

private:
    friend class FunctionData;

    static void make_def_use_chain(Instruction* inst);
};


static_assert(CodeBlock<BasicBlock>, "assumed to be");