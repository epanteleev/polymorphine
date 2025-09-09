#pragma once
#include "LIRInstructionBase.h"


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

    void add_user(LIRInstructionBase *inst) noexcept {
        m_used_in.push_back(inst);
    }

    void kill_user(LIRInstructionBase *inst) noexcept {
        std::erase(m_used_in, inst);
    }

    [[nodiscard]]
    std::span<LIRInstructionBase * const> users() const noexcept {
        return m_used_in;
    }

protected:
    void add_def(const LIRVal& def) {
        m_defs.push_back(def);
    }

    std::vector<LIRVal> m_defs;
    std::vector<LIRInstructionBase *> m_used_in;
};
