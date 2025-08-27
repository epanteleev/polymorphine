#pragma once

namespace aasm {
    class Label final {
    public:
        constexpr explicit Label(const std::uint32_t id) noexcept :
            m_id(id) {}

        [[nodiscard]]
        constexpr std::uint32_t id() const noexcept { return m_id; }

        friend std::ostream& operator<<(std::ostream& os, const Label& label);

    private:
        std::uint32_t m_id;
    };

    inline std::ostream & operator<<(std::ostream &os, const Label &label) {
        return os << 'L' << label.id()+1;
    }
}