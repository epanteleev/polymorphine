#pragma once

/**
 * Represents a live range of a virtual register in the LIR inside a one basic block.
 */
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