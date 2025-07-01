#pragma once

#include "ir_frwd.h"

enum class AnalysisType {
    PreOrderTraverse,
    PostOrderTraverse,
    BFSTraverse,
    LinearScanOrderTraverse,
    DominatorTree,
    LoopNestedForest,
    LinearScan,
    LivenessAnalysis,
    Max
};

class AnalysisPassResult {
public:
    virtual ~AnalysisPassResult() = default;
};

template <typename A>
concept Analysis = requires(A a)
{
    typename A::result_type;
    typename A::basic_block;
    A::analysis_kind;
    a.run();
};