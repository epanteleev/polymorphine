#pragma once

#include "dom/DominatorTreeEvalBase.h"
#include "traverse/BFSOrderTraverseBase.h"
#include "traverse/PostOrderTraverseBase.h"
#include "traverse/PreorderTraverseBase.h"


using BFSOrderTraverse = BFSOrderTraverseBase<FunctionData>;
using PostOrderTraverse = PostOrderTraverseBase<FunctionData>;
using PreorderTraverse = PreorderTraverseBase<FunctionData>;
using DominatorTreeEval = DominatorTreeEvalBase<FunctionData>;

