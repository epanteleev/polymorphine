#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "LIRInstructionBase.h"
#include "pass/Constrains.h"
#include "utility/OrderedSet.h"


class MachBlock final {
public:
    explicit MachBlock(std::size_t id) noexcept
        : m_id(id) {}

    template<std::derived_from<LIRInstructionBase> U>
    U* inst(const LIRInstBuilder<U>& fn) {
        auto creator = [&] (const std::size_t id) {
            return fn(id, this, m_builder);
        };

        return m_instructions.push_back<U>(creator);
    }

    [[nodiscard]]
    std::size_t id() const { return m_id; }

    [[nodiscard]]
    LIRControlInstruction* last() const;

    [[nodiscard]]
    std::span<MachBlock* const> successors() const {
        return last()->successors();
    }

    [[nodiscard]]
    std::span<MachBlock* const> predecessors() const {
        return m_predecessors;
    }

    void print(std::ostream &os) const;

private:
    const std::size_t m_id;
    VregBuilder m_builder;
    std::vector<MachBlock *> m_predecessors;
    OrderedSet<LIRInstructionBase> m_instructions;
};

static_assert(CodeBlock<MachBlock>, "assumed to be");