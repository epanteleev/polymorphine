#pragma once

#include <cstddef>
#include <vector>

#include "lir/x64/lir_frwd.h"
#include "lir/x64/instruction/LIRInstructionBase.h"


class LIRControlInstruction: public LIRInstructionBase {
public:
    explicit LIRControlInstruction(std::vector<LIROperand>&& uses, std::vector<LIRBlock* >&& successors) :
        LIRInstructionBase(std::move(uses)),
        m_successors(std::move(successors)) {}

    [[nodiscard]]
    std::span<LIRBlock * const> successors() const noexcept {
        return m_successors;
    }

    [[nodiscard]]
    const LIRBlock* succ(const std::size_t idx) const {
        assertion(idx < m_successors.size(), "invariant");
        return m_successors[idx];
    }

protected:
    std::vector<LIRBlock* > m_successors;
};