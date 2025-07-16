#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/operand/LIRValMap.h"

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

    [[nodiscard]]
    std::uint32_t start() const noexcept {
        return m_start;
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
    explicit LiveIntervals(LIRValMap<std::vector<Interval>>&& intervals) noexcept:
        m_intervals(std::move(intervals)) {}

    friend std::ostream& operator<<(std::ostream& os, const LiveIntervals& intervals);

    std::span<Interval const> intervals(const LIRVal& val) const noexcept {
        return m_intervals.at(val);
    }

    const LIRValMap<std::vector<Interval>>& intervals() const noexcept {
        return m_intervals;
    }

private:
    LIRValMap<std::vector<Interval>> m_intervals{};
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
