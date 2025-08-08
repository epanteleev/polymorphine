#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/operand/LIRValMap.h"

#include "LiveRange.h"
#include "LiveInterval.h"

class LiveIntervals final: public AnalysisPassResult {
public:
    explicit LiveIntervals(LIRValMap<LiveInterval>&& intervals) noexcept:
        m_intervals(std::move(intervals)) {}

    friend std::ostream& operator<<(std::ostream& os, const LiveIntervals& intervals);

    const LiveInterval& intervals(const LIRVal& val) const noexcept {
        return m_intervals.at(val);
    }

    const LIRValMap<LiveInterval>& intervals() const noexcept {
        return m_intervals;
    }

private:
    LIRValMap<LiveInterval> m_intervals{};
};

inline std::ostream & operator<<(std::ostream &os, const LiveIntervals &intervals) {
    for (const auto& [lir_val, live_interval] : intervals.m_intervals) {
        os << lir_val << " -> ";
        for (const auto& interval: live_interval) {
            os << interval << " ";
        }
    }

    return os;
}
