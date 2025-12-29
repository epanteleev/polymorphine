#pragma once
#include <cstddef>

class InstLocation final {
    static constexpr auto NO_VALUE = std::numeric_limits<std::size_t>::max();
public:
    explicit InstLocation() = default;

    explicit InstLocation(const std::size_t bb_idx, const std::size_t inst_idx) noexcept:
        m_bb_idx(bb_idx),
        m_instruction_idx(inst_idx) {}

    [[nodiscard]]
    std::size_t bb_idx() const noexcept { return m_bb_idx; }
    [[nodiscard]]
    std::size_t inst_idx() const noexcept { return m_instruction_idx; }

    template<typename Os>
    friend Os& operator<<(Os& os, const InstLocation& loc) noexcept {
        return os << '%' << loc.m_bb_idx << 'x' << loc.m_instruction_idx;
    }

private:
    const std::size_t m_bb_idx{NO_VALUE};
    const std::size_t m_instruction_idx{NO_VALUE};
};