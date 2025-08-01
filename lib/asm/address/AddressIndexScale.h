#pragma once

namespace aasm {
    class AddressIndexScale final {
    public:
        explicit constexpr AddressIndexScale(const GPReg base, const GPReg index, const std::uint8_t scale = 1, const std::int32_t displacement = 0) noexcept:
            m_displacement(displacement),
            m_base(base),
            m_index(index),
            scale(scale) {}

        friend std::ostream& operator<<(std::ostream & os, const AddressIndexScale & addr);

        bool operator==(const AddressIndexScale & other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_displacement == other.m_displacement &&
                   m_base == other.m_base &&
                   m_index == other.m_index &&
                   scale == other.scale;
        }

        template<CodeBuffer C>
        void encode(C& c, unsigned int modrm_pattern) const {
            const auto has_sib = m_index.code() || reg3(m_base) == reg3(rsp) || scale > 1;

            /* Explicit m_displacement must be used with BP or R13. */
            const auto has_displacement = m_displacement || m_base == rbp;

            // Emit ModR/M byte: [7-6]=mod, [5-3]=reg, [2-0]=r/m or 4 if SIB present
            std::uint8_t modrm = ((modrm_pattern & 0x7) << 3) | 4;

            // Set Mod bits according to m_displacement presence and range
            if (!std::in_range<std::int8_t>(m_displacement)) {
                // 0b10: 32-bit m_displacement
                modrm |= 0x80;
            } else if (has_displacement) {
                // 0b01: 8-bit m_displacement
                modrm |= 0x40;
            }
            c.emit8(modrm);

            /* SIB */
            if (has_sib) {
                // SIB: [7-6] scale, [5-3] index, [2-0] m_base
                std::uint8_t sib = (scale == 2 ? 1 :
                        scale == 4 ? 2 :
                        scale == 8 ? 3 : 0) << 6;
                sib |= reg3(m_index) << 3 | reg3(m_base);
                c.emit8(sib);
            }

            /* Displacement */
            if (!std::in_range<int8_t>(m_displacement)) {
                c.emit32(m_displacement);
            } else if (has_displacement) {
                c.emit8(m_displacement);
            }
        }

        [[nodiscard]]
        GPReg index() const noexcept {
            return m_index;
        }

    private:
        friend class Address;
        std::int32_t m_displacement;
        GPReg m_base;
        GPReg m_index;
        std::uint8_t scale;
    };

    inline std::ostream& operator<<(std::ostream & os, const AddressIndexScale & addr) {
        if (addr.m_displacement != 0) {
            os << addr.m_displacement;
        }
        os << '(';
        os << '%' << addr.m_base.name(8);
        os << ',';

        os << '%' << addr.m_index.name(8);
        if (addr.scale != 1) {
            os << ',' << static_cast<std::int32_t>(addr.scale);
        }
        return os << ')';
    }
}