#pragma once

#include <vector>

#include "base/Constrains.h"

template<CodeBlock BB>
class Ordering final: public AnalysisPassResult {
public:
    using const_iterator = std::vector<BB*>::const_iterator;

    [[nodiscard]]
    explicit Ordering(std::vector<BB*> &&ordering) noexcept:
        m_ordering(std::move(ordering)) {}

    [[nodiscard]]
    const_iterator begin() const noexcept { return m_ordering.begin(); }
    [[nodiscard]]
    const_iterator end() const noexcept { return m_ordering.end(); }

    [[nodiscard]]
    std::size_t size() const noexcept { return m_ordering.size(); }

    const BB* operator[](std::size_t i) const noexcept { return m_ordering[i]; }

private:
    std::vector<BB*> m_ordering;
};