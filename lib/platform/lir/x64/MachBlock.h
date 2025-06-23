#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "AnyLIRInstruction.h"
#include "module/CodeBlock.h"
#include "utility/OrderedSet.h"


class MachBlock final {
public:
    explicit MachBlock(std::size_t id) noexcept
        : m_id(id) {}

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


private:
    const std::size_t m_id;
    std::vector<MachBlock *> m_predecessors;
    OrderedSet<AnyLIRInstruction> m_instructions;
};

static_assert(CodeBlock<MachBlock>, "assumed to be");