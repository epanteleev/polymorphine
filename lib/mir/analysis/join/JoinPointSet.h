#pragma once

#include "JoinPointSetResult.h"
#include "base/analysis/AnalysisPassManagerBase.h"
#include "base/analysis/dom/DominatorTree.h"
#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "mir/module/BasicBlock.h"
#include "mir/module/FunctionData.h"
#include "mir/instruction/Alloc.h"


class JoinPointSet final {
public:
    using basic_block = BasicBlock;
    using result_type = JoinPointSetResult;

private:
    explicit JoinPointSet(const DominatorTree<BasicBlock>* m_dom_tree) noexcept:
        m_dom_tree(m_dom_tree) {}

public:
    static constexpr auto analysis_kind = AnalysisType::JoinPointSet;

    void run();

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<JoinPointSetResult>(std::move(m_join_set));
    }

    static JoinPointSet create(AnalysisPassManagerBase<FunctionData>* cache, const FunctionData *data) {
        const auto dom = cache->analyze<DominatorTreeEvalBase<FunctionData>>(data);
        return JoinPointSet(dom);
    }

private:
    static bool has_user_in_block(const BasicBlock* block, const Alloc* alloc) noexcept;

    const DominatorTree<BasicBlock>* m_dom_tree;
    std::unordered_map<BasicBlock*, std::unordered_set<Alloc*>> m_join_set{};
};