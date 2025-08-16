#pragma once

#include <span>

#include "utility/OrderedSet.h"

#include "base/BasicBlockBase.h"
#include "base/Constrains.h"

#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/instruction/LIRControlInstruction.h"

class LIRBlock final: public BasicBlockBase<LIRBlock, LIRInstructionBase> {
public:
    template<std::derived_from<LIRInstructionBase> U>
    U* ins(std::unique_ptr<U>&& inst) {
        auto inst_ptr = inst.get();
        const auto id = m_instructions.push_back(std::move(inst));
        inst_ptr->connect(id, this);

        make_def_use_chain(inst_ptr);
        if constexpr (std::derived_from<U, LIRControlInstruction>) {
            make_edges(inst_ptr);
        }
        return inst_ptr;
    }

    template<std::derived_from<LIRInstructionBase> U>
    U* ins_before(const LIRInstructionBase* base, std::unique_ptr<U>&& inst) {
        const auto inst_ptr = inst.get();
        const auto id = m_instructions.insert_before(base->id(), std::move(inst));
        inst_ptr->connect(id, this);

        make_def_use_chain(inst_ptr);
        if constexpr (std::derived_from<U, LIRControlInstruction>) {
            make_edges(base);
        }
        return inst_ptr;
    }

    [[nodiscard]]
    const LIRControlInstruction* last() const;

    [[nodiscard]]
    std::span<LIRBlock* const> successors() const {
        return last()->successors();
    }

    [[nodiscard]]
    const LIRBlock* succ(const std::size_t idx) const {
        return last()->succ(idx);
    }

private:
    friend class LIRFuncData;

    static void make_def_use_chain(LIRInstructionBase* inst);
};

static_assert(CodeBlock<LIRBlock>, "assumed to be");