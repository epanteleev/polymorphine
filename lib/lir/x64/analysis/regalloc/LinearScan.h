#pragma once
#include "RegisterAllocation.h"


class LinearScan final {
public:
    using result_type = RegisterAllocation;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::LinearScan;

    void run() {

    }

    std::unique_ptr<result_type> result() noexcept {
        return {};
    }

    static LinearScan create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        const auto loop_info = cache->analyze<LoopInfoEvalBase<ObjFuncData>>(data);
        return {};
    }
};
