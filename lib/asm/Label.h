#pragma once

#include <cstdint>

namespace aasm {
    class Label final {
    public:
        constexpr explicit Label(const std::uint32_t id) noexcept:
            m_id(id) {}

        [[nodiscard]]
        constexpr std::uint32_t id() const noexcept { return m_id; }

        template<typename Os>
        friend Os& operator<<(Os& os, const Label& label) {
            return os << 'L' << label.id()+1;
        }

    private:
        std::uint32_t m_id;
    };
}