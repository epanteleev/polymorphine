#pragma once
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCacheBase.h"
#include "platform/lir/x64/MachBlock.h"
#include "platform/lir/x64/ObjFuncData.h"


class LivenessAnalysis final {
public:
    using result_type = LivenessAnalysis;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::LivenessAnalysis;

    void run() {

    }

    std::shared_ptr<result_type> result() noexcept {
        return {};
    }

    static LivenessAnalysis create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        return {};
    }
};
