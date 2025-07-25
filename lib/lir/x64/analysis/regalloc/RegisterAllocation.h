#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/asm/GPVReg.h"


class RegisterAllocation final: public AnalysisPassResult {
public:
    explicit RegisterAllocation(LIRValMap<GPVReg>&& reg_allocation, std::int32_t local_area_size) noexcept:
        m_reg_allocation(std::move(reg_allocation)),
        m_local_area_size(local_area_size) {}

    friend std::ostream& operator<<(std::ostream& os, const RegisterAllocation& regs);

    const GPVReg& operator[](const LIRVal& val) const noexcept {
        return m_reg_allocation.at(val);
    }

    [[nodiscard]]
    std::int32_t local_area_size() const noexcept {
        return m_local_area_size;
    }

private:
    LIRValMap<GPVReg> m_reg_allocation;
    std::int32_t m_local_area_size;
};

inline std::ostream & operator<<(std::ostream &os, const RegisterAllocation &regs) {
    for (const auto &[val, reg] : regs.m_reg_allocation) {
        os << val << " -> " << reg << '\n';
    }

    return os;
}
