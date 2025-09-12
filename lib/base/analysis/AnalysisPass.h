#pragma once

enum class AnalysisType {
    PreOrderTraverse,
    PostOrderTraverse,
    BFSTraverse,
    DominatorTree,
    LivenessAnalysis,
    LiveIntervalsEval,
    LiveIntervalsGroups,
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
    a.result();
    A::create;
};