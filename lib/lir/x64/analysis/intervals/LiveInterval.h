#pragma once

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
    bool follows(const LiveInterval& other) const noexcept {
        if (this == &other) { return false; }
        if (intersects(other)) {
            return false;
        }

        const auto pred = [&](const LiveRange& interval) {
            return interval.end() == other.start() || interval.start() == other.finish();
        };

        return std::ranges::any_of(m_intervals, pred); //TODO 'binary search'???
    }

    /**
     * Merges this interval with another interval.
     * The intervals may overlap.
     */
    void merge_with(const LiveInterval& other) noexcept {
        if (this == &other) { return; }

        for (const auto& interval : other.m_intervals) {
            m_intervals.emplace_back(interval);
        }

        canonicalize(m_intervals);
    }

    /**
     * Creates a LiveInterval from a vector of LiveRange.
     * The vector must not be empty.
     */
    static LiveInterval create(std::vector<LiveRange>&& intervals) noexcept {
        assertion(!intervals.empty(), "LiveInterval must have at least one interval");

        const auto max = canonicalize(intervals);
        return LiveInterval(std::move(intervals), max);
    }

    friend std::ostream& operator<<(std::ostream& os, const LiveInterval& interval);

private:
    explicit LiveInterval(std::vector<LiveRange>&& intervals, const std::uint32_t end) noexcept:
        m_intervals(std::move(intervals)),
        m_finish(end) {}

    /**
     * 1. Sorts the intervals by their start point.
     * 2. Finds the maximum end point of the intervals.
     */
    static std::uint32_t canonicalize(std::vector<LiveRange>& intervals) noexcept {
        const auto sorted_intervals = [](const LiveRange& lhs, const LiveRange& rhs) {
            return lhs.start() < rhs.start();
        };

        std::ranges::sort(intervals, sorted_intervals);

        const auto max_elem = [](const LiveRange& lhs, const LiveRange& rhs) {
            return lhs.end() < rhs.end();
        };

        const auto max_end = std::ranges::max_element(intervals, max_elem);
        return max_end->end();
    }

    // Sorted by start point set of intervals.
    std::vector<LiveRange> m_intervals;
    std::uint32_t m_finish;
};

inline std::ostream & operator<<(std::ostream &os, const LiveInterval &interval) {
    for (const auto& range : interval) {
        os << range << " ";
    }

    return os;
}
