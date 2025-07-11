#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/asm/GPVReg.h"


class RegisterAllocation final: public AnalysisPassResult {
public:
    explicit RegisterAllocation(LIRValMap<GPVReg>&& reg_allocation) noexcept:
        m_reg_allocation(std::move(reg_allocation)) {}

    friend std::ostream& operator<<(std::ostream& os, const RegisterAllocation& regs);

    const GPVReg& operator[](const LIRVal& val) const noexcept {
        return m_reg_allocation.at(val);
    }

private:
    LIRValMap<GPVReg> m_reg_allocation;
};

inline std::ostream & operator<<(std::ostream &os, const RegisterAllocation &regs) {
    for (const auto &[val, reg] : regs.m_reg_allocation) {
        os << val << " -> " << reg << '\n';
    }

    return os;
}
