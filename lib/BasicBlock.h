#pragma once

#include <vector>
#include <iosfwd>

#include "instruction/Instruction.h"
#include "instruction/Terminator.h"
#include "utility/OrderedSet.h"

class BasicBlock final {
public:
    explicit BasicBlock(const std::size_t id): m_id(id) {}

    template<typename U>
    requires(std::convertible_to<U*, Instruction*>)
    U* push_back(const InstructionBuilder<U>& fn) {
        auto creator = [&] (std::size_t id) {
            return fn(id, this);
        };

        auto inst = m_instructions.push_back<U>(creator);
        make_def_use_chain(inst);
        make_edges(inst);
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

    void print(std::ostream &os) const;

    void print_short_name(std::ostream &os) const;

private:
    static void make_def_use_chain(Instruction* inst);
    static void make_edges(Instruction* inst);

private:
    const std::size_t m_id;
    std::vector<BasicBlock *> m_predecessors;
    OrderedSet<Instruction> m_instructions;
};
