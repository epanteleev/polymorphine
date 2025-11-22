#pragma once
#include <iosfwd>

namespace aasm::details {
    static constexpr std::uint8_t CDQ = 0x99;

    class Cdq final {
    public:
        explicit constexpr Cdq(const std::uint8_t size) noexcept:
            m_size(size) {}

        friend std::ostream &operator<<(std::ostream &out, const Cdq &cdq);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer &buffer) const {
            switch (m_size) {
                case 2: [[fallthrough]];
                case 4: buffer.emit8(CDQ); break;
                case 8: buffer.emit8(constants::REX_W); buffer.emit8(CDQ); break;
                default: die("Invalid operand size for cdq instruction: {}", static_cast<unsigned>(m_size));
            }
            return std::nullopt;
        }

    private:
        const std::uint8_t m_size;
    };
}