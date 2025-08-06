#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/operand/LIRValMap.h"
#include "lir/x64/asm/GPVReg.h"


class FixedRegisters final: public AnalysisPassResult {
public:
    using const_iterator = LIRValMap<aasm::GPReg>::const_iterator;

    explicit FixedRegisters(LIRValMap<aasm::GPReg>&& rax_map, std::vector<aasm::GPReg>&& arg_map) noexcept:
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

    std::span<aasm::GPReg const> arguments() const noexcept {
        return m_args_map;
    }

    std::optional<aasm::GPReg> get(const LIRVal& val) const noexcept {
        if (const auto it = m_reg_map.find(val); it != m_reg_map.end()) {
            return it->second;
        }

        return std::nullopt;
    }

private:
    // Maps LIR values to fixed registers. It also contains argument values.
    const LIRValMap<aasm::GPReg> m_reg_map;
    // Maps argument values to registers
    const std::vector<aasm::GPReg> m_args_map;
};

inline std::ostream& operator<<(std::ostream &os, const FixedRegisters &regs) {
    for (const auto& [val, reg] : regs.m_reg_map) {
        os << val << "-> " << reg << ' ';
    }

    return os;
}
