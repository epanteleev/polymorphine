#pragma once

#include "IntervalHint.h"
#include "LiveRange.h"
#include "utility/RemoveFast.h"

/**
 * Represents a live interval of a virtual register in LIR function.
 * A live interval is a set of LiveRange intervals that are sorted by their start point.
 */
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
    IntervalHint hint() const noexcept {
        return m_hint;
    }

    /**
     * Returns the number of intervals.
     */
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
    bool follows(const LiveInterval& other) const noexcept {
        if (this == &other) { return false; }
        if (intersects(other)) {
            return false;
        }

        const auto pred = [&](const LiveRange& interval) {
            return interval.end() == other.start();
        };

        return std::ranges::any_of(m_intervals, pred); //TODO 'binary search'???
    }

    /**
     * Merges this interval with another interval.
     * The intervals may overlap.
     */
    void merge_with(const LiveInterval& other) noexcept {
        if (this == &other) { return; }

        m_intervals.reserve(m_intervals.size() + other.m_intervals.size());
        for (const auto& interval : other.m_intervals) {
            m_intervals.emplace_back(interval);
        }

        m_hint = join_hints(m_hint, other.hint());
        m_finish = canonicalize(m_intervals);
    }

    /**
     * Creates a LiveInterval from a vector of LiveRange.
     * The vector must not be empty.
     */
    static LiveInterval create(std::vector<LiveRange>&& intervals, const IntervalHint hint) noexcept {
        assertion(!intervals.empty(), "LiveInterval must have at least one interval");

        const auto max = canonicalize(intervals);
        return LiveInterval(std::move(intervals), max, hint);
    }

    friend std::ostream& operator<<(std::ostream& os, const LiveInterval& interval);

private:
    explicit LiveInterval(std::vector<LiveRange>&& intervals, const std::uint32_t end, const IntervalHint hint) noexcept:
        m_intervals(std::move(intervals)),
        m_finish(end),
        m_hint(hint) {}

    /**
     * Transforms the intervals into a canonical form. This involves:
     * 1. Sorts the intervals by their start point.
     * 2. Merges overlapping intervals.
     * 3. Finds the maximum end point of the intervals.
     */
    [[nodiscard]]
    static std::uint32_t canonicalize(std::vector<LiveRange>& intervals) noexcept {
        const auto sorted_intervals = [](const LiveRange& lhs, const LiveRange& rhs) {
            return lhs.start() < rhs.start();
        };

        std::ranges::sort(intervals, sorted_intervals);

        // Merge overlapping intervals
        for (std::size_t i = 1; i < intervals.size();) {
            if (intervals[i].intersects_non_strictly(intervals[i - 1])) {
                intervals[i - 1].propagate(intervals[i].end());
                remove_fast(intervals, intervals.begin() + static_cast<std::int64_t>(i));
            } else {
                i += 1;
            }
        }
        std::ranges::sort(intervals, sorted_intervals);

        const auto max_elem = [](const LiveRange& lhs, const LiveRange& rhs) {
            return lhs.end() < rhs.end();
        };

        const auto max_end = std::ranges::max_element(intervals, max_elem);
        return max_end->end();
    }

    std::vector<LiveRange> m_intervals;
    std::uint32_t m_finish;
    IntervalHint m_hint;
};

inline std::ostream & operator<<(std::ostream &os, const LiveInterval &interval) {
    for (const auto& range : interval) {
        os << range << " ";
    }

    return os;
}
