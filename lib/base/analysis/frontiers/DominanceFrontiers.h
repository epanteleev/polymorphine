#pragma once

#include <optional>
#include <unordered_map>
#include <vector>

#include "base/analysis/AnalysisPass.h"
#include "base/Constrains.h"

template<CodeBlock BB>
class DominanceFrontiers final: public AnalysisPassResult {
public:
    using frontier_set = std::vector<BB*>;
    using frontier_map = std::unordered_map<BB*, frontier_set>;

    explicit DominanceFrontiers(frontier_map &&frontiers) noexcept:
        m_frontiers(std::move(frontiers)) {}

    [[nodiscard]]
    std::optional<std::span<BB* const>> frontiers(BB* const bb) const {
        const auto frontiers = m_frontiers.find(bb);
        if (frontiers == m_frontiers.end()) {
            return std::nullopt;
        }

        return frontiers->second;
    }

private:
    frontier_map m_frontiers;
};