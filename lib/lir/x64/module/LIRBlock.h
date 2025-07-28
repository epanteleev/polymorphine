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
    template<std::derived_from<LIRInstructionBase> U>
    U* inst(std::unique_ptr<U>&& inst) {
        auto inst_ptr = inst.get();
        const auto id = m_instructions.push_back(std::move(inst));
        inst_ptr->connect(id, this);
        return inst_ptr;
    }

    [[nodiscard]]
    LIRControlInstruction* last() const;

    [[nodiscard]]
    std::span<LIRBlock* const> successors() const {
        return last()->successors();
    }

    friend class LIRFuncData;
};

static_assert(CodeBlock<LIRBlock>, "assumed to be");