#pragma once

#include <unordered_set>

#include "base/Constrains.h"

template<CodeBlock BB>
class DominanceFrontiersIterator final {
public:
    using frontier_set = std::unordered_set<BB*>;
    using iterator = frontier_set::const_iterator;

    explicit DominanceFrontiersIterator(const std::unordered_set<BB*> &bbs) noexcept:
        m_dominance_frontiers(bbs) {}

    [[nodiscard]]
    iterator begin() const noexcept {
        return m_dominance_frontiers.begin();
    }

    [[nodiscard]]
    iterator end() const noexcept {
        return m_dominance_frontiers.end();
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_dominance_frontiers.size();
    }

    [[nodiscard]]
    bool contains(BB* const bb) const noexcept {
        return m_dominance_frontiers.contains(bb);
    }

private:
    const std::unordered_set<BB*>& m_dominance_frontiers;
};