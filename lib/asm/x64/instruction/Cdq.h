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
        constexpr void emit(Buffer &buffer) const {
            switch (m_size) {
                case 2: [[fallthrough]];
                case 4: buffer.emit8(CDQ); break;
                case 8: buffer.emit8(constants::REX_W); buffer.emit8(CDQ); break;
                default: die("Invalid operand size for cdq instruction: {}", static_cast<unsigned>(m_size));
            }
        }

    private:
        const std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &out, const Cdq &cdq) {
        switch (cdq.m_size) {
            case 2: return out << "cwtd";
            case 4: return out << "cdtq";
            case 8: return out << "cqto";
            default: die("Invalid operand size for cdq instruction: {}", static_cast<unsigned>(cdq.m_size));
        }
    }
}