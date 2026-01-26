#pragma once
#include <cstdint>

enum class AnalysisType: std::uint8_t {
    PreOrderTraverse,
    PostOrderTraverse,
    BFSTraverse,
    DominatorTree,
    LivenessAnalysis,
    LiveIntervalsEval,
    LiveIntervalsGroups,
    JoinPointSet,
    EscapeAnalysis,
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