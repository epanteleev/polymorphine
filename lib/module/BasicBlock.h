#pragma once

#include <vector>
#include <iosfwd>

#include "../pass/Constrains.h"
#include "instruction/Instruction.h"
#include "instruction/Terminator.h"
#include "utility/OrderedSet.h"

class BasicBlock final {
public:
    explicit BasicBlock(const std::size_t id): m_id(id) {}

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
    std::size_t id() const { return m_id; }

    [[nodiscard]]
    Terminator last() const;

    [[nodiscard]]
    std::span<BasicBlock* const> successors() const {
        return last().targets();
    }

    [[nodiscard]]
    std::span<BasicBlock* const> predecessors() const {
        return m_predecessors;
    }

    [[nodiscard]]
    const OrderedSet<Instruction>& instructions() const noexcept {
        return m_instructions;
    }

    void print(std::ostream &os) const;
    void print_short_name(std::ostream &os) const;

private:
    static void make_def_use_chain(Instruction* inst);

    template<IsTerminator T>
    static void make_edges(T *inst)  {
        for (auto block: inst->successors()) {
            block->m_predecessors.push_back(inst->owner());
        }
    }

private:
    const std::size_t m_id;
    std::vector<BasicBlock *> m_predecessors;
    OrderedSet<Instruction> m_instructions;
};


static_assert(CodeBlock<BasicBlock>, "assumed to be");