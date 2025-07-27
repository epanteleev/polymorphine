#pragma once

#include "base/analysis/AnalysisPass.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/operand/LIRValMap.h"

class LiveRange final {
public:
    LiveRange(const std::uint32_t start, const std::uint32_t end) noexcept:
        m_start(start),
        m_end(end) {}

    [[nodiscard]]
    bool intersects(const LiveRange& other) const noexcept {
        return !(m_start > other.m_end || m_end < other.m_start);
    }

    void propagate(const std::uint32_t end) noexcept {
        m_end = std::max(m_end, end);
    }

    [[nodiscard]]
    std::uint32_t start() const noexcept {
        return m_start;
    }

    [[nodiscard]]
    std::uint32_t end() const noexcept {
        return m_end;
    }

    friend std::ostream& operator<<(std::ostream& os, const LiveRange& interval);

private:
    std::uint32_t m_start;
    std::uint32_t m_end;
};

inline std::ostream& operator<<(std::ostream& os, const LiveRange& interval) {
    return os << '[' << interval.m_start << ", " << interval.m_end << ']';
}

class LiveInterval final {
public:
    using const_iterator = std::vector<LiveRange>::const_iterator;

    [[nodiscard]]
    const_iterator begin() const noexcept {
        return m_intervals.begin();
    }

    [[nodiscard]]
    const_iterator end() const noexcept {
        return m_intervals.end();
    }

    [[nodiscard]]
    bool intersects(const LiveInterval& other) const noexcept {
        if (m_start > other.m_end || m_end < other.m_start) {
            return false;
        }

        for (const auto& interval : m_intervals) {
            for (const auto& other_interval : other.m_intervals) {
                if (interval.intersects(other_interval)) return true;
            }
        }

        return false;
    }

    static LiveInterval create(std::vector<LiveRange>&& intervals) noexcept {
        const auto start = intervals.front().start();
        const auto end = intervals.back().end();
        return LiveInterval(std::move(intervals), start, end);
    }

private:
    explicit LiveInterval(std::vector<LiveRange>&& intervals, const std::uint32_t start, const std::uint32_t end) noexcept:
        m_start(start),
        m_end(end),
        m_intervals(std::move(intervals)) {}

    std::uint32_t m_start;
    std::uint32_t m_end;
    std::vector<LiveRange> m_intervals;
};

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
