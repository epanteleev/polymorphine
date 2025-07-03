#pragma once
#include "pass/analysis/AnalysisPass.h"
#include "platform/lir/x64/MachBlock.h"
#include "platform/lir/x64/VRegMap.h"


class LiveInfo final {
public:
    LiveInfo(VRegSet&& live_in, VRegSet&& live_out):
        m_live_in(std::move(live_in)),
        m_live_out(std::move(live_out)) {}

    auto live_in() const noexcept {
        return m_live_in;
    }

    auto live_out() const noexcept {
        return m_live_out;
    }

private:
    VRegSet m_live_in;
    VRegSet m_live_out;
};


class LivenessAnalysisInfo final: public AnalysisPassResult {
public:
    explicit LivenessAnalysisInfo(std::unordered_map<const MachBlock*, LiveInfo>&& liveness) noexcept:
        m_liveness(std::move(liveness)) {}

private:
    std::unordered_map<const MachBlock*, LiveInfo> m_liveness;
};