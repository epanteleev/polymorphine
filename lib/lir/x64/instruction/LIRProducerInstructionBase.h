#pragma once
#include "LIRInstruction.h"


class LIRProducerInstructionBase : public LIRInstructionBase {
public:
    explicit LIRProducerInstructionBase(std::vector<LIROperand> &&uses) noexcept:
        LIRInstructionBase(std::move(uses)) {}

    [[nodiscard]]
    std::span<LIRVal const> defs() const noexcept {
        return m_defs;
    }

    [[nodiscard]]
    const LIRVal& def(const std::size_t idx) const {
        return m_defs.at(idx);
    }

protected:
    void add_def(const LIRVal& def) {
        m_defs.push_back(def);
    }

    std::vector<LIRVal> m_defs;
};
