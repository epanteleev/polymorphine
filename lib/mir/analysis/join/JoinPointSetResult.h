#pragma once
#include <unordered_map>
#include <unordered_set>

#include "base/analysis/AnalysisPass.h"
#include "mir/module/BasicBlock.h"

class JoinPointSetResult final: public AnalysisPassResult {
public:
    explicit JoinPointSetResult(std::unordered_map<const BasicBlock*, std::unordered_set<const Alloc*>>&& joins) noexcept:
        m_join_set(std::move(joins)) {}

private:
    std::unordered_map<const BasicBlock*, std::unordered_set<const Alloc*>> m_join_set;
};
