#pragma once

#include "DominanceFrontiers.h"
#include "base/analysis/AnalysisPassManagerBase.h"

template<Function FD>
class DominanceFrontiersEvalBase final {
public:
    using basic_block = FD::code_block_type;
    using result_type = DominanceFrontiers<basic_block>;

private:
    explicit DominanceFrontiersEvalBase(const DominatorTree<basic_block>& dominator_tree) noexcept:
        m_dominator_tree(dominator_tree) {}

public:
    static constexpr auto analysis_kind = AnalysisType::DominatorTree;

    void run() {
        init_frontier_set();
        construct_frontier_set();
    }

    static DominanceFrontiersEvalBase create(AnalysisPassManagerBase<FD> *cache, const FD *data) {
        const auto dominator_tree = cache->template analyze<DominatorTreeEvalBase<FD>>(data);
        return DominanceFrontiersEvalBase(*dominator_tree);
    }

    std::unique_ptr<result_type> result() {
        return std::make_unique<result_type>(std::move(m_dominance_frontiers));
    }

private:
    using frontier_set = DominanceFrontiers<basic_block>::frontier_set;
    using frontier_map = DominanceFrontiers<basic_block>::frontier_map;

    void init_frontier_set() {
        const auto idoms = m_dominator_tree.immediate_dominators();
        for (const auto [block, sub_tree]: idoms) {
            m_dominance_frontiers.emplace(block, frontier_set{});
        }
    }

    void construct_frontier_set() {
        for (auto [block, b_idom]: m_dominator_tree.immediate_dominators()) {
            auto preds = block->predecessors();
            if (preds.size() < 2) {
                continue;
            }

            for (const auto& pred: preds) {
                auto runner = pred;
                while (runner != b_idom) {
                    auto it = m_dominance_frontiers.find(runner);
                    if (it != m_dominance_frontiers.end()) {
                        it->second.push_back(block);
                    }
                    it->second.push_back(block);

                    const auto dom_it = m_dominator_tree.immediate_dominator(block);
                    if (!dom_it.has_value()) {
                        break;
                    }

                    runner = dom_it.value();
                }
            }
        }
    }

    frontier_map m_dominance_frontiers;
    const DominatorTree<basic_block>& m_dominator_tree;
};