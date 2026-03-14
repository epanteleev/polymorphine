#pragma once

#include "JoinPointSetResult.h"

#include "base/analysis/AnalysisPassManagerBase.h"
#include "base/analysis/dom/DominatorTree.h"
#include "base/analysis/frontiers/DominanceFrontiers.h"

#include "mir/analysis/escape/EscapeAnalysisResult.h"
#include "mir/module/BasicBlock.h"
#include "mir/module/FunctionData.h"

class JoinPointSet final {
public:
    using basic_block = BasicBlock;
    using result_type = JoinPointSetResult;

private:
    explicit JoinPointSet(const DominanceFrontiers<BasicBlock>& frontiers, const EscapeAnalysisResult* escape_analysis, const FunctionData *data) noexcept:
        m_frontiers(frontiers),
        m_data(data),
        m_escape_analysis(escape_analysis) {}

public:
    static constexpr auto analysis_kind = AnalysisType::JoinPointSet;

    void run();

    [[nodiscard]]
    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<JoinPointSetResult>(std::move(m_join_set));
    }

    [[nodiscard]]
    static JoinPointSet create(AnalysisPassManagerBase<FunctionData>* cache, const FunctionData *data);

private:
    static bool has_user_in_block(const BasicBlock* block, const Alloc* alloc) noexcept;

    void evaluate_joins(const Alloc* alloc, std::unordered_set<BasicBlock*>&& stores) noexcept;
    void add_value(BasicBlock* bb, const Alloc* alloc) noexcept;

    const DominanceFrontiers<BasicBlock>& m_frontiers;
    const FunctionData *m_data;
    const EscapeAnalysisResult* m_escape_analysis;
    std::unordered_map<BasicBlock*, std::vector<const Alloc*>> m_join_set;
};