#pragma once

#include "intervals/LiveIntervalsEval.h"
#include "liveness/LivenessAnalysis.h"

#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "base/analysis/traverse/BFSOrderTraverseBase.h"
#include "base/analysis/traverse/PostOrderTraverseBase.h"
#include "base/analysis/traverse/PreorderTraverseBase.h"

#include "lir/x64/module/ObjFuncData.h"
#include "lir/x64/analysis/regalloc/LinearScan.h"
#include "regalloc/FixedRegistersEval.h"


using BFSOrderTraverseMach = BFSOrderTraverseBase<ObjFuncData>;
using PostOrderTraverseMach = PostOrderTraverseBase<ObjFuncData>;
using PreorderTraverseMach = PreorderTraverseBase<ObjFuncData>;
using DominatorTreeEvalMach = DominatorTreeEvalBase<ObjFuncData>;

static_assert(Analysis<BFSOrderTraverseMach>);
static_assert(Analysis<PostOrderTraverseMach>);
static_assert(Analysis<PreorderTraverseMach>);
static_assert(Analysis<DominatorTreeEvalMach>);
static_assert(Analysis<LinearScan>);
static_assert(Analysis<LivenessAnalysis>);
static_assert(Analysis<LiveIntervalsEval>);
static_assert(Analysis<FixedRegistersEval>);

using AnalysisPassCacheMach = AnalysisPassCacheBase<ObjFuncData>;
