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
        return !(m_start >= other.m_end || m_end <= other.m_start);
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
    std::uint32_t start() const noexcept {
        return m_intervals.front().start();
    }

    [[nodiscard]]
    std::uint32_t finish() const noexcept {
        return m_intervals.back().end();
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_intervals.size();
    }

    [[nodiscard]]
    bool intersects(const LiveInterval& other) const noexcept {
        if (this == &other) { return true; }
        if (start() >= other.finish() || finish() <= other.start()) {
            return false;
        }

        // TODO improve algo
        for (const auto& interval : m_intervals) {
            for (const auto& other_interval : other.m_intervals) {
                if (interval.intersects(other_interval)) return true;
            }
        }

        return false;
    }

    /**
     * Returns true if this interval is bordering the other interval.
     */
    [[nodiscard]]
    bool follows_to(const LiveInterval& other) const noexcept {
        if (this == &other) { return false; }
        if (intersects(other)) {
            return false;
        }

        const auto pred = [&](const LiveRange& interval) {
            return interval.end() == other.start();
        };

        return std::ranges::any_of(m_intervals, pred); //TODO 'binary search'???
    }

    void merge_with(const LiveInterval& other) noexcept {
        if (this == &other) { return; }

        for (const auto& interval : other.m_intervals) {
            m_intervals.emplace_back(interval);
        }

        const auto sorter = [](const LiveRange& interval1, const LiveRange& interval2) {
            return interval1.start() < interval2.start();
        };

        std::ranges::sort(m_intervals, sorter);
    }

    static LiveInterval create(std::vector<LiveRange>&& intervals) noexcept {
        assertion(!intervals.empty(), "LiveInterval must have at least one interval");

        const auto sorted_intervals = [](const LiveRange& lhs, const LiveRange& rhs) {
            return lhs.start() < rhs.start();
        };

        std::ranges::sort(intervals, sorted_intervals);


        const auto max_elem = [](const LiveRange& lhs, const LiveRange& rhs) {
            return lhs.end() < rhs.end();
        };

        const auto max_end = std::ranges::max_element(intervals, max_elem);
        return LiveInterval(std::move(intervals), max_end->end());
    }

private:
    explicit LiveInterval(std::vector<LiveRange>&& intervals, std::uint32_t end) noexcept:
        m_intervals(std::move(intervals)),
        m_finish(end) {}

    std::vector<LiveRange> m_intervals;
    std::uint32_t m_finish;
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
