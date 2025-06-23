#pragma once

#include "pass/analysis/dom/DominatorTreeEvalBase.h"
#include "pass/analysis/traverse/BFSOrderTraverseBase.h"
#include "pass/analysis/traverse/PostOrderTraverseBase.h"
#include "pass/analysis/traverse/PreorderTraverseBase.h"

#include "platform/lir/x64/MachBlock.h"


using BFSOrderTraverseMach = BFSOrderTraverseBase<MachBlock>;
using PostOrderTraverse = PostOrderTraverseBase<MachBlock>;
using PreorderTraverse = PreorderTraverseBase<MachBlock>;
using DominatorTreeEval = DominatorTreeEvalBase<MachBlock>;
