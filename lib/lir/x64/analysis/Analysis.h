#pragma once

#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "base/analysis/traverse/BFSOrderTraverseBase.h"
#include "base/analysis/traverse/PostOrderTraverseBase.h"
#include "base/analysis/traverse/PreorderTraverseBase.h"

#include "lir/x64/module/LIRFuncData.h"
#include "lir/x64/analysis/regalloc/LinearScanBase.h"
#include "lir/x64/analysis/fixedregs/FixedRegistersEvalBase.h"
#include "lir/x64/analysis/join_intervals/LiveIntervalsJoinEval.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/analysis/liveness/LivenessAnalysis.h"
#include "lir/x64/asm/cc/LinuxX64.h"

using BFSOrderTraverseLIR = BFSOrderTraverseBase<LIRFuncData>;
using PostOrderTraverseLIR = PostOrderTraverseBase<LIRFuncData>;
using PreorderTraverseLIR = PreorderTraverseBase<LIRFuncData>;
using DominatorTreeEvalLIR = DominatorTreeEvalBase<LIRFuncData>;

using LinearScanLinuxX64 = LinearScanBase<call_conv::LinuxX64>;
using FixedRegistersEvalLinuxX64 = FixedRegistersEvalBase<call_conv::LinuxX64>;
using LiveIntervalsJoinEvalLinux64 = LiveIntervalsJoinEval<call_conv::LinuxX64>;

static_assert(Analysis<BFSOrderTraverseLIR>);
static_assert(Analysis<PostOrderTraverseLIR>);
static_assert(Analysis<PreorderTraverseLIR>);
static_assert(Analysis<DominatorTreeEvalLIR>);
static_assert(Analysis<LinearScanLinuxX64>);
static_assert(Analysis<LivenessAnalysis>);
static_assert(Analysis<LiveIntervalsEval>);
static_assert(Analysis<FixedRegistersEvalLinuxX64>);
static_assert(Analysis<LiveIntervalsJoinEvalLinux64>);

using LIRAnalysisPassManager = AnalysisPassManagerBase<LIRFuncData>;
