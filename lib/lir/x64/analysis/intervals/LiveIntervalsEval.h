#pragma once

#include "LiveIntervals.h"
#include "base/analysis/AnalysisPass.h"
#include "lir/x64/module/MachBlock.h"
#include "lir/x64/module/ObjFuncData.h"


class LiveIntervalsEval final {
public:
    using result_type = LiveIntervals;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::LiveIntervalsEval;

private:
    explicit LiveIntervalsEval(const ObjFuncData& objFuncData) noexcept:
        m_obj_func_data(objFuncData) {}

public:
    void run() {

    }

    std::unique_ptr<LiveIntervals> result() {
        return std::make_unique<LiveIntervals>();
    }

    static LiveIntervalsEval create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        return LiveIntervalsEval(*data);
    }

private:


private:
    const ObjFuncData& m_obj_func_data;
};
