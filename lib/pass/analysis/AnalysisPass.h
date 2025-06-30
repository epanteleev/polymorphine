#pragma once

#include <concepts>

#include "ir_frwd.h"

enum class AnalysisType {
    PreOrderTraverse,
    PostOrderTraverse,
    BFSTraverse,
    DominatorTree,
    LoopNestedForest,
    Max
};

class AnalysisPassResult {
public:
    virtual ~AnalysisPassResult() = default;
};

template <typename A>
concept Analysis = std::derived_from<A, AnalysisPass> && requires(A a)
{
    typename A::result_type;
    typename A::basic_block;
    A::analysis_kind;
};

class AnalysisPass {
public:
    explicit AnalysisPass(const FunctionData* data) :
        m_data(data) {}

    virtual ~AnalysisPass() = default;
    virtual void run() = 0;

protected:
    const FunctionData* m_data;
};