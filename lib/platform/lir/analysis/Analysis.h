#pragma once

#include "pass/analysis/dom/DominatorTreeEvalBase.h"
#include "pass/analysis/traverse/BFSOrderTraverseBase.h"
#include "pass/analysis/traverse/PostOrderTraverseBase.h"
#include "pass/analysis/traverse/PreorderTraverseBase.h"

#include "platform/lir/x64/MachBlock.h"
#include "platform/lir/x64/ObjFuncData.h"


using BFSOrderTraverseMach = BFSOrderTraverseBase<ObjFuncData>;
using PostOrderTraverseMach = PostOrderTraverseBase<ObjFuncData>;
using PreorderTraverseMach = PreorderTraverseBase<ObjFuncData>;
using DominatorTreeEvalMach = DominatorTreeEvalBase<ObjFuncData>;
