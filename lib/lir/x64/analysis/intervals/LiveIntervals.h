#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/module/MachBlock.h"
#include "lir/x64/operand/LIRVRegMap.h"

class Interval final {
public:
    Interval(std::uint32_t start, std::uint32_t end) noexcept:
        m_start(start),
        m_end(end) {}

    [[nodiscard]]
    bool intersects(const Interval& other) const noexcept {
        return m_start <= other.m_start && m_end >= other.m_end;
    }

    void propagate(const std::uint32_t end) noexcept {
        m_end = std::max(m_end, end);
    }


    friend std::ostream& operator<<(std::ostream& os, const Interval& interval);

private:
    std::uint32_t m_start;
    std::uint32_t m_end;
};

inline std::ostream& operator<<(std::ostream& os, const Interval& interval) {
    return os << '[' << interval.m_start << ", " << interval.m_end << ']';
}

class LiveIntervals final: public AnalysisPassResult {
public:
    explicit LiveIntervals(LIRVRegMap<std::unordered_map<const MachBlock*, Interval>>&& intervals) noexcept:
        m_intervals(std::move(intervals)) {}

    friend std::ostream& operator<<(std::ostream& os, const LiveIntervals& intervals);

private:
    LIRVRegMap<std::unordered_map<const MachBlock*, Interval>> m_intervals{};
};

inline std::ostream & operator<<(std::ostream &os, const LiveIntervals &intervals) {
    for (const auto& [vreg, intervals] : intervals.m_intervals) {
        os << vreg << " -> ";
        for (const auto& [bb, interval]: intervals) {
            os << "{ ";
            bb->print_short_name(os);
            os << ": " << interval << " }";
        }
    }

    return os;
}
