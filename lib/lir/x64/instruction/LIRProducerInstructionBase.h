#pragma once
#include "LIRInstructionBase.h"
#include "lir/x64/asm/GPVReg.h"


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

    void assign_reg(const std::size_t idx, const OptionalGPVReg& reg) {
        assertion(idx < m_assigned_regs.size(), "Index out of bounds");
        assertion(!m_assigned_regs[idx].to_gp_op().has_value(), "Register already assigned");
        m_assigned_regs[idx] = reg;
    }

    [[nodiscard]]
    const OptionalGPVReg& assigned_reg(const std::size_t idx) const {
        assertion(idx < m_assigned_regs.size(), "Index out of bounds");
        return m_assigned_regs.at(idx);
    }

    [[nodiscard]]
    std::span<LIRInstructionBase * const> users() const noexcept {
        return m_used_in;
    }

protected:
    void add_def(const LIRVal& def) {
        m_defs.push_back(def);
        m_assigned_regs.emplace_back();
    }

    std::vector<LIRVal> m_defs;
    std::vector<OptionalGPVReg> m_assigned_regs;
    std::vector<LIRInstructionBase *> m_used_in;
};
