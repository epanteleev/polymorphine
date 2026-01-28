#pragma once

#include "EscapeAnalysisResult.h"
#include "EscapeState.h"
#include "base/analysis/AnalysisPass.h"
#include "base/analysis/AnalysisPassManagerBase.h"
#include "base/analysis/traverse/Ordering.h"
#include "base/analysis/traverse/PreorderTraverseBase.h"
#include "mir/module/BasicBlock.h"
#include "mir/module/FunctionData.h"

class EscapeAnalysis final {
public:
    using basic_block = BasicBlock;
    using result_type = EscapeAnalysisResult;

private:
    explicit EscapeAnalysis(const Ordering<basic_block>& ordering) noexcept:
        m_ordering(ordering) {}

public:
    static constexpr auto analysis_kind = AnalysisType::EscapeAnalysis;

    void run();

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<EscapeAnalysisResult>(std::move(m_escape_state));
    }

    static EscapeAnalysis create(AnalysisPassManagerBase<FunctionData>* cache, const FunctionData *data) {
        const auto& preorder = *cache->analyze<PreorderTraverseBase<FunctionData>>(data);
        return EscapeAnalysis(preorder);
    }

private:
    const Ordering<basic_block>& m_ordering;
    std::unordered_map<const ValueInstruction*, EscapeState> m_escape_state;
};