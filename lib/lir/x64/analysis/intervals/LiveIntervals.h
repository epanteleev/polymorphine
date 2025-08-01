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

    std::span<LiveRange const> intervals(const LIRVal& val) const noexcept {
        return m_intervals.at(val);
    }

    const LIRValMap<LiveInterval>& intervals() const noexcept {
        return m_intervals;
    }

private:
    LIRValMap<LiveInterval> m_intervals{};
};

inline std::ostream & operator<<(std::ostream &os, const LiveIntervals &intervals) {
    for (const auto& [vreg, intervals] : intervals.m_intervals) {
        os << vreg << " -> ";
        for (const auto& interval: intervals) {
            os << interval << " ";
        }
    }

    return os;
}
