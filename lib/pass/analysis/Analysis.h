#pragma once

#include "dom/DominatorTreeEvalBase.h"
#include "traverse/BFSOrderTraverseBase.h"
#include "traverse/PostOrderTraverseBase.h"
#include "traverse/PreorderTraverseBase.h"

#include "module/BasicBlock.h"


using BFSOrderTraverse = BFSOrderTraverseBase<BasicBlock>;
using PostOrderTraverse = PostOrderTraverseBase<BasicBlock>;
using PreorderTraverse = PreorderTraverseBase<BasicBlock>;
using DominatorTreeEval = DominatorTreeEvalBase<BasicBlock>;

