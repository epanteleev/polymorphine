#pragma once

#include <vector>

#include "ir_frwd.h"
#include <iosfwd>

#include "pass/analysis/AnalysisPass.h"

class Ordering final: public AnalysisPassResult {
public:
    using const_iterator = std::vector<BasicBlock*>::const_iterator;

    [[nodiscard]]
    explicit Ordering(std::vector<BasicBlock*> &&ordering)
        : m_ordering(std::move(ordering)) {}

    [[nodiscard]]
    const_iterator begin() const noexcept { return m_ordering.begin(); }
    [[nodiscard]]
    const_iterator end() const noexcept { return m_ordering.end(); }

    [[nodiscard]]
    std::size_t size() const noexcept { return m_ordering.size(); }

private:
    std::vector<BasicBlock*> m_ordering;
};

std::ostream& operator<<(std::ostream &os, const Ordering& ordering);


