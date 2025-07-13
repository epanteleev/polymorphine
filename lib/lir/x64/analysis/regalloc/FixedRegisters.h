#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/operand/LIRValMap.h"


class FixedRegisters final: public AnalysisPassResult {
public:
    explicit FixedRegisters(LIRValSet&& rax_set) noexcept:
        m_rax_set(rax_set) {}

    friend std::ostream& operator<<(std::ostream& os, const FixedRegisters& regs);

    [[nodiscard]]
    const LIRValSet& rax_set() const noexcept {
        return m_rax_set;
    }

private:
    const LIRValSet m_rax_set;
};

inline std::ostream& operator<<(std::ostream &os, const FixedRegisters &regs) {
    os << "rax: ";
    for (const auto &reg : regs.m_rax_set) {
        os << reg << " ";
    }

    return os;
}
