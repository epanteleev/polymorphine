#pragma once

#include "pass/analysis/Analysis.h"
#include "pass/analysis/AnalysisPass.h"
#include "pass/analysis/AnalysisPassCacheBase.h"
#include "platform/lir/x64/MachBlock.h"
#include "platform/lir/x64/ObjFuncData.h"


class LivenessAnalysis final {
public:
    using result_type = LivenessAnalysis;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::LivenessAnalysis;

private:
    explicit LivenessAnalysis(const Ordering<MachBlock> &linear_scan_order):
        m_linear_scan_order(linear_scan_order) {}

public:
    void run() {

    }

    std::unique_ptr<result_type> result() noexcept {
        return {};
    }

    static LivenessAnalysis create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        auto linear_scan_order = cache->analyze<LinearScanOrder>();
        return {};
    }

private:
    const Ordering<MachBlock>& m_linear_scan_order;

};
