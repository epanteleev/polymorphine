#pragma once

#include <unordered_map>

#include "DominanceFrontiersIterator.h"
#include "base/analysis/AnalysisPass.h"
#include "base/Constrains.h"

template<CodeBlock BB>
class DominanceFrontiers final: public AnalysisPassResult {
public:
    using frontier_set = std::unordered_set<BB*>;
    using frontier_map = std::unordered_map<BB*, frontier_set>;

    explicit DominanceFrontiers(frontier_map &&frontiers) noexcept:
        m_frontiers(std::move(frontiers)) {}

    [[nodiscard]]
    DominanceFrontiersIterator<BB> frontiers(BB* const bb) const {
        const auto frontiers = m_frontiers.find(bb);
        if (frontiers == m_frontiers.end()) {
            static std::unordered_set<BB*> empty;
            return DominanceFrontiersIterator<BB>(empty);
        }

        return DominanceFrontiersIterator<BB>(frontiers->second);
    }

private:
    frontier_map m_frontiers;
};