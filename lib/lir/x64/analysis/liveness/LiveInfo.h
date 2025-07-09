#pragma once

#include "base/analysis/AnalysisPass.h"

#include "lir/x64/module/MachBlock.h"
#include "lir/x64/operand/VRegMap.h"


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
    const VRegSet m_live_in;
    const VRegSet m_live_out;
};


class LivenessAnalysisInfo final: public AnalysisPassResult {
public:
    explicit LivenessAnalysisInfo(std::unordered_map<const MachBlock*, LiveInfo>&& liveness) noexcept:
        m_liveness(std::move(liveness)) {}

private:
    std::unordered_map<const MachBlock*, LiveInfo> m_liveness;
};