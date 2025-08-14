#pragma once

#include "ClobberRegs.h"
#include "base/analysis/AnalysisPass.h"
#include "lir/x64/asm/GPVReg.h"
#include "lir/x64/operand/LIRValMap.h"


class RegisterAllocation final: public AnalysisPassResult {
public:
    explicit RegisterAllocation(std::unordered_map<const LIRInstructionBase*, ClobberRegs>&& clobber_regs, LIRValMap<GPVReg>&& reg_allocation, std::vector<aasm::GPReg>&& used_callee_saved_regs, const std::int32_t local_area_size) noexcept:
        m_clobber_regs(clobber_regs),
        m_reg_allocation(std::move(reg_allocation)),
        m_used_callee_saved_regs(std::move(used_callee_saved_regs)),
        m_local_area_size(local_area_size) {}

    friend std::ostream& operator<<(std::ostream& os, const RegisterAllocation& regs);

    const GPVReg& operator[](const LIRVal& val) const noexcept {
        return m_reg_allocation.at(val);
    }

    [[nodiscard]]
    std::int32_t local_area_size() const noexcept {
        return m_local_area_size;
    }

    [[nodiscard]]
    std::size_t frame_size() const noexcept {
        return m_used_callee_saved_regs.size() * 8 + m_local_area_size;
    }

    [[nodiscard]]
    std::optional<const ClobberRegs*> try_get_clobber_regs(const LIRInstructionBase* inst) const noexcept {
        if (const auto it = m_clobber_regs.find(inst); it != m_clobber_regs.end()) {
            return &it->second;
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::span<const aasm::GPReg> used_callee_saved_regs() const noexcept {
        return m_used_callee_saved_regs;
    }

private:
    const LIRValMap<GPVReg> m_reg_allocation;
    std::unordered_map<const LIRInstructionBase*, ClobberRegs> m_clobber_regs;
    const std::vector<aasm::GPReg> m_used_callee_saved_regs{};
    const std::int32_t m_local_area_size;
};

inline std::ostream & operator<<(std::ostream &os, const RegisterAllocation &regs) {
    for (const auto &[val, reg] : regs.m_reg_allocation) {
        os << val << " -> " << reg << '\n';
    }

    return os;
}