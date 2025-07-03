
#pragma once
#include <span>
#include <vector>

#include "LIROperand.h"

class Chain final {
public:
    using uses_iterator = std::vector<LIROperand>::iterator;

    Chain(std::vector<LIROperand>&& uses, std::vector<VReg>&& defs) noexcept:
        m_defs(std::move(defs)),
        m_uses(std::move(uses)) {}

    void add_use(const LIROperand& use);

    void add_def(const VReg& def);

    [[nodiscard]]
    std::span<LIROperand const> uses() const noexcept {
        return m_uses;
    }

    [[nodiscard]]
    const LIROperand& use(const std::size_t idx) const {
        return m_uses.at(idx);
    }

    [[nodiscard]]
    std::span<VReg const> defs() const {
        return m_defs;
    }

    [[nodiscard]]
    const VReg& def(const std::size_t idx) const {
        return m_defs.at(idx);
    }

private:
    std::vector<VReg> m_defs;
    std::vector<LIROperand> m_uses;
};
