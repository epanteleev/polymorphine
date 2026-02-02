#pragma once

#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "base/analysis/frontiers/DominanceFrontiersEvalBase.h"
#include "base/analysis/traverse/BFSOrderTraverseBase.h"
#include "base/analysis/traverse/PostOrderTraverseBase.h"
#include "base/analysis/traverse/PreorderTraverseBase.h"
#include "escape/EscapeAnalysis.h"
#include "join/JoinPointSet.h"

#include "mir/module/FunctionData.h"

using BFSOrderTraverse = BFSOrderTraverseBase<FunctionData>;
using PostOrderTraverse = PostOrderTraverseBase<FunctionData>;
using PreorderTraverse = PreorderTraverseBase<FunctionData>;
using DominatorTreeEval = DominatorTreeEvalBase<FunctionData>;
using DominanceFrontiersEval = DominanceFrontiersEvalBase<FunctionData>;

static_assert(Analysis<BFSOrderTraverse>);
static_assert(Analysis<PostOrderTraverse>);
static_assert(Analysis<PreorderTraverse>);
static_assert(Analysis<DominatorTreeEval>);
static_assert(Analysis<JoinPointSet>);
static_assert(Analysis<EscapeAnalysis>);
static_assert(Analysis<DominanceFrontiersEval>);

using AnalysisPassManager = AnalysisPassManagerBase<FunctionData>;