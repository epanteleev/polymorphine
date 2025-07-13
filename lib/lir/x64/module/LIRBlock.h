#pragma once

#include <cstddef>
#include <span>

#include "base/BasicBlockBase.h"
#include "base/Constrains.h"

#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/instruction/LIRControlInstruction.h"

#include "utility/OrderedSet.h"


class LIRBlock final: public BasicBlockBase<LIRBlock, LIRInstructionBase> {
public:
    explicit LIRBlock(const std::size_t id) noexcept: BasicBlockBase(id) {}

    template<std::derived_from<LIRInstructionBase> U>
    U* inst(const LIRInstBuilder<U>& fn) {
        auto creator = [&] (const std::size_t id) {
            return fn(id, this);
        };

        return m_instructions.push_back<U>(creator);
    }

    [[nodiscard]]
    LIRControlInstruction* last() const;

    [[nodiscard]]
    std::span<LIRBlock* const> successors() const {
        return last()->successors();
    }
};

static_assert(CodeBlock<LIRBlock>, "assumed to be");