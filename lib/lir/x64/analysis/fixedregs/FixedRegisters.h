#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/operand/LIRValMap.h"
#include "lir/x64/asm/GPVReg.h"


class FixedRegisters final: public AnalysisPassResult {
public:
    using const_iterator = LIRValMap<GPVReg>::const_iterator;

    explicit FixedRegisters(LIRValMap<GPVReg>&& rax_map, std::vector<GPVReg>&& arg_map) noexcept:
        m_reg_map(std::move(rax_map)),
        m_args_map(std::move(arg_map)) {}

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

    std::span<GPVReg const> arguments() const noexcept {
        return m_args_map;
    }

private:
    // Maps LIR values to fixed registers. It also contains argument values.
    const LIRValMap<GPVReg> m_reg_map;
    // Maps argument values to registers
    const std::vector<GPVReg> m_args_map;
};

inline std::ostream& operator<<(std::ostream &os, const FixedRegisters &regs) {
    for (const auto& [val, reg] : regs.m_reg_map) {
        os << val << "-> " << reg;
    }

    return os;
}
