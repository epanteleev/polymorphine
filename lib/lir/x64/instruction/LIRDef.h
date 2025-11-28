#pragma once

#include <span>
#include "lir/x64/operand/LIRVal.h"
#include "utility/InplaceVec.h"

class LIRDef {
public:
    explicit LIRDef(const LIRValType type) noexcept:
        m_type(type) {}

    [[nodiscard]]
    LIRValType type() const noexcept {
        return m_type;
    }

    [[nodiscard]]
    std::span<LIRVal const> defs() const noexcept {
        return m_defs.span();
    }

    [[nodiscard]]
    const LIRVal& def(const std::size_t idx) const {
        return m_defs[idx];
    }

    void assign_reg(const std::size_t idx, const AssignedVReg& reg) {
        assertion(idx < m_assigned_regs.size(), "Index out of bounds");
        assertion(m_assigned_regs[idx].empty(), "Register already assigned");
        m_assigned_regs[idx] = reg;
    }

    [[nodiscard]]
    const AssignedVReg& assigned_reg(const std::size_t idx) const {
        assertion(idx < m_assigned_regs.size(), "Index out of bounds");
        return m_assigned_regs[idx];
    }

protected:
    void add_def(const LIRVal& def) {
        m_defs.push_back(def);
        m_assigned_regs.emplace_back();
    }

private:
    InplaceVec<LIRVal, 2> m_defs;
    InplaceVec<AssignedVReg, 2> m_assigned_regs;
    LIRValType m_type;
};