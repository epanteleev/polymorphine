#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/operand/LIRValMap.h"
#include "lir/x64/asm/GPVReg.h"


class FixedRegisters final: public AnalysisPassResult {
public:
    using const_iterator = LIRValMap<GPVReg>::const_iterator;

    explicit FixedRegisters(LIRValMap<GPVReg>&& rax_map) noexcept:
        m_reg_map(rax_map) {}

    friend std::ostream& operator<<(std::ostream& os, const FixedRegisters& regs);

    const_iterator begin() const noexcept {
        return m_reg_map.begin();
    }

    const_iterator end() const noexcept {
        return m_reg_map.end();
    }

    [[nodiscard]]
    bool contains(const LIRVal& val) const noexcept {
        return m_reg_map.contains(val);
    }

private:
    const LIRValMap<GPVReg> m_reg_map;
};

inline std::ostream& operator<<(std::ostream &os, const FixedRegisters &regs) {
    for (const auto& [val, reg] : regs.m_reg_map) {
        os << val << "-> " << reg;
    }

    return os;
}
