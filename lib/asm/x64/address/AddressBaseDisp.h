#pragma once

namespace aasm {
    class AddressBaseDisp final {
    public:
        explicit AddressBaseDisp(const GPReg base, const std::int32_t displacement = 0) noexcept:
            m_displacement(displacement),
            m_base(base) {}

        bool operator==(const AddressBaseDisp & other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_displacement == other.m_displacement && m_base == other.m_base;
        }

        friend std::ostream& operator<<(std::ostream & os, const AddressBaseDisp & addr);

        template<CodeBuffer C>
        std::optional<Relocation> encode(C& c, unsigned int modrm_pattern) const {
            const auto has_displacement = m_displacement || m_base == rbp;

            // Emit ModR/M byte: [7-6]=mod, [5-3]=reg, [2-0]=r/m or 4 if SIB present
            std::uint8_t modrm = ((modrm_pattern & 0x7) << 3) | (m_base == rsp ? 4 : reg3(m_base));

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
            if (m_base == rsp) {
                // SIB: [7-6] scale, [5-3] index, [2-0] m_base
                c.emit8(reg3(rsp) << 3 | reg3(m_base));
            }

            /* Displacement */
            if (!std::in_range<int8_t>(m_displacement)) {
                c.emit32(m_displacement);
            } else if (has_displacement) {
                c.emit8(m_displacement);
            }

            return std::nullopt;
        }

        [[nodiscard]]
        std::int32_t offset() const noexcept {
            return m_displacement;
        }

        [[nodiscard]]
        AddressBaseDisp add_offset(const std::int32_t offset) const noexcept {
            const auto new_offset = static_cast<std::int64_t>(offset) + m_displacement;
            return AddressBaseDisp(m_base, aasm::checked_cast<std::int32_t>(new_offset));
        }

    private:
        friend class Address;
        std::int32_t m_displacement;
        GPReg m_base;
    };

    std::ostream& operator<<(std::ostream & os, const AddressBaseDisp & addr);
}
