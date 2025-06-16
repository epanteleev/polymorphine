#pragma once

#include <memory>
#include <unordered_map>

#include "DominatorTree.h"
#include "pass/analysis/AnalysisPass.h"
#include "ir_frwd.h"
#include "pass/analysis/traverse/Ordering.h"


class DominatorTreeEval final: public AnalysisPass {
    DominatorTreeEval(const FunctionData* data, Ordering& postorder) noexcept
        : AnalysisPass(data),
        m_postorder(postorder) {}

public:
    using result_type = DominatorTree;
    static constexpr auto analysis_kind = AnalysisType::DominatorTree;

    void run() override;

    static DominatorTreeEval create(AnalysisPassCache* cache, const FunctionData* data);

    std::shared_ptr<DominatorTree> result() noexcept;

private:
    void enumeration_to_dom_map(const Ordering& ordering, const std::unordered_map<std::size_t, BasicBlock*>& index_to_block, std::unordered_map<std::size_t, std::size_t>& dominators);

    std::unordered_map<BasicBlock*, std::unique_ptr<DominatorNode>> dominator_tree;
    Ordering& m_postorder;
};
