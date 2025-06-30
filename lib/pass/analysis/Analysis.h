#pragma once

#include "dom/DominatorTreeEvalBase.h"
#include "loop/LoopInfoEvalBase.h"
#include "traverse/BFSOrderTraverseBase.h"
#include "traverse/LinearScanOrderBase.h"
#include "traverse/PostOrderTraverseBase.h"
#include "traverse/PreorderTraverseBase.h"


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