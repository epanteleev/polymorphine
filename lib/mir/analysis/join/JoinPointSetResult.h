#pragma once
#include <unordered_map>
#include <unordered_set>

#include "base/analysis/AnalysisPass.h"
#include "mir/module/BasicBlock.h"

class JoinPointSetResult final: public AnalysisPassResult {
public:
    using join_set = std::unordered_map<BasicBlock*, std::unordered_set<const Alloc*>>;
    using iterator = join_set::const_iterator;

    explicit JoinPointSetResult(std::unordered_map<BasicBlock*, std::unordered_set<const Alloc*>>&& joins) noexcept:
        m_join_set(std::move(joins)) {}

    iterator begin() const noexcept {
        return m_join_set.begin();
    }

    iterator end() const noexcept {
        return m_join_set.end();
    }

private:
    std::unordered_map<BasicBlock*, std::unordered_set<const Alloc*>> m_join_set;
};