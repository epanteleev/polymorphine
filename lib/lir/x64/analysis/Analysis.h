#pragma once

#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "base/analysis/traverse/BFSOrderTraverseBase.h"
#include "base/analysis/traverse/PostOrderTraverseBase.h"
#include "base/analysis/traverse/PreorderTraverseBase.h"

#include "lir/x64/module/LIRFuncData.h"
#include "lir/x64/analysis/regalloc/LinearScan.h"
#include "lir/x64/analysis/regalloc/FixedRegistersEval.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/analysis/liveness/LivenessAnalysis.h"

using BFSOrderTraverseLIR = BFSOrderTraverseBase<LIRFuncData>;
using PostOrderTraverseLIR = PostOrderTraverseBase<LIRFuncData>;
using PreorderTraverseLIR = PreorderTraverseBase<LIRFuncData>;
using DominatorTreeEvalLIR = DominatorTreeEvalBase<LIRFuncData>;

static_assert(Analysis<BFSOrderTraverseLIR>);
static_assert(Analysis<PostOrderTraverseLIR>);
static_assert(Analysis<PreorderTraverseLIR>);
static_assert(Analysis<DominatorTreeEvalLIR>);
static_assert(Analysis<LinearScan>);
static_assert(Analysis<LivenessAnalysis>);
static_assert(Analysis<LiveIntervalsEval>);
static_assert(Analysis<FixedRegistersEval>);

using AnalysisPassManagerMach = AnalysisPassManagerBase<LIRFuncData>;
