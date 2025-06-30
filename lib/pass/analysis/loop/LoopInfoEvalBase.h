#pragma once

#include "LoopInfoBase.h"
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCacheBase.h"
#include "pass/analysis/dom/DominatorTreeEvalBase.h"
#include "pass/analysis/traverse/Ordering.h"
#include "utility/Error.h"

template<Function FD>
class LoopInfoEvalBase final {
public:
    using basic_block = typename FD::code_block_type;
    using result_type = LoopInfoBase<basic_block>;

private:
    LoopInfoEvalBase(DominatorTree<basic_block>& dominator_tree, Ordering<basic_block>& postorder):
        m_dominator_tree(dominator_tree),
        m_postorder(postorder) {}

public:
    static constexpr auto analysis_kind = AnalysisType::LoopNestedForest;

    void run() {
        for (const auto bb: m_postorder) {
            for (const auto p: bb->predecessors()) {
                if (!m_dominator_tree.dominates(bb, p)) {
                    continue;
                }

                auto loop_body = get_loop_body(bb, p);
                const auto exit = get_exit_block(loop_body);
                const auto enter = get_enter_block(bb, loop_body);
                std::vector<LoopBlock<basic_block>> vec;
                vec.emplace_back(exit, enter, std::move(loop_body));
                m_loops.emplace(bb, std::move(vec));
            }
        }
    }

    std::unordered_set<basic_block*> get_loop_body(basic_block* header, basic_block* predecessor) {
        std::unordered_set<basic_block*> loop_body;
        loop_body.insert(predecessor);

        std::stack<basic_block*> worklist;
        worklist.push(predecessor);
        while (!worklist.empty()) {
            const auto bb = worklist.top();
            worklist.pop();

            if (bb == header) {
                continue;
            }

            if (!m_dominator_tree.dominates(header, bb)) {
                continue;
            }

            if (auto res = loop_body.emplace(bb); !res.second) {
                continue;
            }

            for (auto p: bb->predecessors()) {
                worklist.push(p);
            }
        }

        return loop_body;
    }

    basic_block* get_exit_block(const std::unordered_set<basic_block*>& body) {
        for (auto bb : body) {
            for (auto s: bb->successors()) {
                if (!body.contains(s)) {
                    return bb;
                }
            }
        }

        die("unreachable");
    }

    basic_block* get_enter_block(basic_block* header, const std::unordered_set<basic_block*>& body) {
        for (auto s: header->successors()) {
            if (body.contains(s)) return s;
        }

        die("unreachable");
    }

    static LoopInfoEvalBase create(AnalysisPassCacheBase<FD>* cache, const FD *data) {
        auto dominator_tree = cache->template analyze<DominatorTreeEvalBase<FD>>(data);
        auto postorder = cache->template analyze<PostOrderTraverseBase<FD>>(data);

        return {*dominator_tree, *postorder};
    }

    std::shared_ptr<result_type> result() noexcept {
        return std::make_shared<result_type>(std::move(m_loops));
    }

private:
    DominatorTree<basic_block>& m_dominator_tree;
    Ordering<basic_block>& m_postorder;
    std::unordered_map<basic_block*, std::vector<LoopBlock<basic_block>>> m_loops;
};
