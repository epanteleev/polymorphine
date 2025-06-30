#pragma once

#include <cstddef>
#include <span>

#include "base/BasicBlockBase.h"
#include "LIRInstructionBase.h"
#include "pass/Constrains.h"
#include "utility/OrderedSet.h"


class MachBlock final: public BasicBlockBase<MachBlock, LIRInstructionBase> {
public:
    explicit MachBlock(const std::size_t id) noexcept: BasicBlockBase(id) {}

    template<std::derived_from<LIRInstructionBase> U>
    U* inst(const LIRInstBuilder<U>& fn) {
        auto creator = [&] (const std::size_t id) {
            return fn(id, this, m_builder);
        };

        return m_instructions.push_back<U>(creator);
    }

    [[nodiscard]]
    LIRControlInstruction* last() const;

    [[nodiscard]]
    std::span<MachBlock* const> successors() const {
        return last()->successors();
    }

private:
    VregBuilder m_builder;
};

static_assert(CodeBlock<MachBlock>, "assumed to be");