#pragma once

#include "base/analysis/AnalysisPass.h"

#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/operand/LIRValMap.h"


class LiveInfo final {
public:
    LiveInfo(LIRValSet&& live_in, LIRValSet&& live_out):
        m_live_in(std::move(live_in)),
        m_live_out(std::move(live_out)) {}

    const LIRValSet& live_in() const noexcept {
        return m_live_in;
    }

    const LIRValSet& live_out() const noexcept {
        return m_live_out;
    }

private:
    const LIRValSet m_live_in;
    const LIRValSet m_live_out;
};


class LivenessAnalysisInfo final: public AnalysisPassResult {
public:
    explicit LivenessAnalysisInfo(std::unordered_map<const LIRBlock*, LiveInfo>&& liveness) noexcept:
        m_liveness(std::move(liveness)) {}

    const LIRValSet& live_in(const LIRBlock* bb) const noexcept {
        return m_liveness.at(bb).live_in();
    }

    const LIRValSet& live_out(const LIRBlock* bb) const noexcept {
        return m_liveness.at(bb).live_out();
    }

    friend std::ostream& operator<<(std::ostream& os, const LivenessAnalysisInfo& info);
private:
    std::unordered_map<const LIRBlock*, LiveInfo> m_liveness;
};

inline std::ostream & operator<<(std::ostream &os, const LivenessAnalysisInfo &info) {
    for (const auto& [bb, live_info]: info.m_liveness) {
        os << "Block " << bb->id() << ": Live In: ";
        for (const auto& reg: live_info.live_in()) {
            os << reg << " ";
        }
        os << "Live Out: ";
        for (const auto& reg: live_info.live_out()) {
            os << reg << " ";
        }
    }
    return os;
}
