
#pragma once
#include <span>
#include <vector>

#include "LIROperand.h"

#include "mach_frwd.h"


class Chain final {
public:
    using uses_iterator = std::vector<LIROperand>::iterator;
private:
    class InputIterator {
    public:
        InputIterator(uses_iterator begin, uses_iterator end):
            m_curr(begin), m_end(end) {}

        InputIterator& operator++() {
            auto vred_opt = m_curr->vreg();
            while (m_curr != m_end && !vred_opt.has_value()) {
                ++m_curr;
                vred_opt = m_curr->vreg();
            }

            return *this;
        }

        VReg operator*() const noexcept {
            return m_curr->vreg().value();
        }

    private:
        uses_iterator m_curr;
        uses_iterator m_end;
    };

public:
    Chain(std::span<LIROperand> uses, std::span<VReg> defs) noexcept:
        m_defs(defs.begin(), defs.end()),
        m_uses(uses.begin(), uses.end()) {}

    void add_use(const LIROperand& use);

    void add_def(const VReg& def);

    InputIterator uses() {
        return {m_uses.begin(), m_uses.end()};
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
