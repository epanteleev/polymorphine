#include "Mem2Reg.h"

#include "mir/analysis/Analysis.h"

void Mem2Reg::run() noexcept {

}

Mem2Reg Mem2Reg::create(FunctionData &fn) noexcept {
    AnalysisPassManager manager;
    const auto join_point_set = manager.analyze<JoinPointSet>(&fn);
    const auto dominator_tree = manager.analyze<DominatorTreeEval>(&fn);

    return Mem2Reg(fn, *dominator_tree, *join_point_set);
}
