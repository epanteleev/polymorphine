#pragma once

#include <vector>
#include <iosfwd>

#include "instruction/Instruction.h"
#include "utility/OrderedSet.h"
#include "value/LocalValue.h"

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

    void print(std::ostream &os) const;

private:
    static void make_def_use_chain(Instruction* inst);
    static void make_edges(Instruction* inst);

private:
    const std::size_t m_id;
    std::vector<BasicBlock *> m_predecessors;
    OrderedSet<Instruction> m_instructions;
};
