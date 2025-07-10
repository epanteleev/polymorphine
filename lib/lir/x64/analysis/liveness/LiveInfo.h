#pragma once

#include "base/analysis/AnalysisPass.h"

#include "lir/x64/module/MachBlock.h"
#include "lir/x64/operand/LIRVRegMap.h"


class LiveInfo final {
public:
    LiveInfo(LIRVRegSet&& live_in, LIRVRegSet&& live_out):
        m_live_in(std::move(live_in)),
        m_live_out(std::move(live_out)) {}

    const LIRVRegSet& live_in() const noexcept {
        return m_live_in;
    }

    const LIRVRegSet& live_out() const noexcept {
        return m_live_out;
    }

private:
    const LIRVRegSet m_live_in;
    const LIRVRegSet m_live_out;
};


class LivenessAnalysisInfo final: public AnalysisPassResult {
public:
    explicit LivenessAnalysisInfo(std::unordered_map<const MachBlock*, LiveInfo>&& liveness) noexcept:
        m_liveness(std::move(liveness)) {}

    const LIRVRegSet& live_in(const MachBlock* bb) const noexcept {
        return m_liveness.at(bb).live_in();
    }

    const LIRVRegSet& live_out(const MachBlock* bb) const noexcept {
        return m_liveness.at(bb).live_out();
    }

private:
    std::unordered_map<const MachBlock*, LiveInfo> m_liveness;
};