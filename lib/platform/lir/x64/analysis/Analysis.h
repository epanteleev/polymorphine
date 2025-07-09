#pragma once

#include "intervals/LiveIntervalsEval.h"
#include "liveness/LivenessAnalysis.h"
#include "pass/analysis/dom/DominatorTreeEvalBase.h"
#include "pass/analysis/traverse/BFSOrderTraverseBase.h"
#include "pass/analysis/traverse/LinearScanOrderBase.h"
#include "pass/analysis/traverse/PostOrderTraverseBase.h"
#include "pass/analysis/traverse/PreorderTraverseBase.h"

#include "../module/ObjFuncData.h"
#include "platform/lir/x64/analysis/regalloc/LinearScan.h"


using BFSOrderTraverseMach = BFSOrderTraverseBase<ObjFuncData>;
using PostOrderTraverseMach = PostOrderTraverseBase<ObjFuncData>;
using PreorderTraverseMach = PreorderTraverseBase<ObjFuncData>;
using DominatorTreeEvalMach = DominatorTreeEvalBase<ObjFuncData>;
using LinearScanOrderMach = LinearScanOrderBase<ObjFuncData>;

static_assert(Analysis<BFSOrderTraverseMach>);
static_assert(Analysis<PostOrderTraverseMach>);
static_assert(Analysis<PreorderTraverseMach>);
static_assert(Analysis<DominatorTreeEvalMach>);
static_assert(Analysis<LinearScanOrderMach>);
static_assert(Analysis<LinearScan>);
static_assert(Analysis<LivenessAnalysis>);
static_assert(Analysis<LiveIntervalsEval>);

using AnalysisPassCacheMach = AnalysisPassCacheBase<ObjFuncData>;
