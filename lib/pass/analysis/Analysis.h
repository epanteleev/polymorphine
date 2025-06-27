#pragma once

#include "dom/DominatorTreeEvalBase.h"
#include "loop/LoopInfoEvalBase.h"
#include "traverse/BFSOrderTraverseBase.h"
#include "traverse/PostOrderTraverseBase.h"
#include "traverse/PreorderTraverseBase.h"


using BFSOrderTraverse = BFSOrderTraverseBase<FunctionData>;
using PostOrderTraverse = PostOrderTraverseBase<FunctionData>;
using PreorderTraverse = PreorderTraverseBase<FunctionData>;
using DominatorTreeEval = DominatorTreeEvalBase<FunctionData>;
using LoopInfoEval = LoopInfoEvalBase<FunctionData>;

static_assert(IsAnalysis<BFSOrderTraverse>);
static_assert(IsAnalysis<PostOrderTraverse>);
static_assert(IsAnalysis<PreorderTraverse>);
static_assert(IsAnalysis<DominatorTreeEval>);
static_assert(IsAnalysis<LoopInfoEval>);