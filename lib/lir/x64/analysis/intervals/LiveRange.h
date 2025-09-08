#pragma once

/**
 * Represents a live range of a virtual register in the LIR inside a one basic block or neighboring blocks.
 */
class LiveRange final {
public:
    LiveRange(const std::uint32_t start, const std::uint32_t end) noexcept:
        m_start(start),
        m_end(end) {}

    /**
     * Checks if two live ranges intersect.
     */
    [[nodiscard]]
    bool intersects(const LiveRange& other) const noexcept {
        return m_start < other.m_end && m_end > other.m_start;
    }

    /**
     * Checks if two live ranges intersect non-strictly.
     * This means that touching intervals are considered intersecting.
     */
    [[nodiscard]]
    bool intersects_non_strictly(const LiveRange& other) const noexcept {
        return m_start <= other.m_end && m_end >= other.m_start;
    }

    /**
     * Propagates the end of the live range to the given end if it is greater than the current end.
     */
    void propagate(const std::uint32_t end) noexcept {
        m_end = std::max(m_end, end);
    }

    /**
     * Returns the start of the live range.
     */
    [[nodiscard]]
    std::uint32_t start() const noexcept {
        return m_start;
    }

    /**
     * Returns the end of the live range.
     */
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