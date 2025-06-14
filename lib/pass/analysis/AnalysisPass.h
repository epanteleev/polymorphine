#pragma once

#include <type_traits>

#include "ir_frwd.h"


class AnalysisPass;

enum class AnalysisType {
    PreorderTraverse,
    PostorderTraverse,
    DominatorTree,
    Max
};

class AnalysisPassResult {
public:
    virtual ~AnalysisPassResult() = default;
};

template <typename A>
concept IsAnalysis = std::is_base_of_v<AnalysisPass, A>;

class AnalysisPass {
public:
    explicit AnalysisPass(const FunctionData* data) :
        m_data(data) {}

    virtual ~AnalysisPass() = default;
    virtual void run() = 0;

protected:
    const FunctionData* m_data;
};