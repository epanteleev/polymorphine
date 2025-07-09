#pragma once

#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "base/analysis/loop/LoopInfoEvalBase.h"
#include "base/analysis/traverse/BFSOrderTraverseBase.h"
#include "base/analysis/traverse/LinearScanOrderBase.h"
#include "base/analysis/traverse/PostOrderTraverseBase.h"
#include "base/analysis/traverse/PreorderTraverseBase.h"

#include "mir/module/FunctionData.h"


using BFSOrderTraverse = BFSOrderTraverseBase<FunctionData>;
using PostOrderTraverse = PostOrderTraverseBase<FunctionData>;
using PreorderTraverse = PreorderTraverseBase<FunctionData>;
using LinearScanOrder = LinearScanOrderBase<FunctionData>;
using DominatorTreeEval = DominatorTreeEvalBase<FunctionData>;
using LoopInfoEval = LoopInfoEvalBase<FunctionData>;

static_assert(Analysis<BFSOrderTraverse>);
static_assert(Analysis<PostOrderTraverse>);
static_assert(Analysis<PreorderTraverse>);
static_assert(Analysis<LinearScanOrder>);
static_assert(Analysis<DominatorTreeEval>);
static_assert(Analysis<LoopInfoEval>);


using AnalysisPassCache = AnalysisPassCacheBase<FunctionData>;