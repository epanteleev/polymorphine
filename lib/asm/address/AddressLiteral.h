#pragma once

namespace aasm {
    class AddressLiteral final {
    public:
        explicit AddressLiteral(const Symbol* symbol, const std::uint32_t displacement) noexcept:
            m_symbol(symbol),
            m_displacement(displacement) {}

        friend std::ostream &operator<<(std::ostream &os, const AddressLiteral &address);

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> encode(Buffer &buffer, const std::uint32_t modrm_pattern) const {
            static constexpr std::uint8_t MODRM = 0x05; // ModR/M byte for direct addressing
            buffer.emit8((modrm_pattern & 0x7) << 3 | MODRM);
            buffer.emit32(INT32_MAX);
            switch (m_symbol->linkage()) {
                case Linkage::EXTERNAL: return Relocation(RelType::R_X86_64_PLT32, buffer.size(), m_displacement, m_symbol);
                case Linkage::INTERNAL: return Relocation(RelType::R_X86_64_PC32, buffer.size(), m_displacement, m_symbol);
                default: die("Unsupported linkage type for AddressLiteral: {}", static_cast<std::uint8_t>(m_symbol->linkage()));
            }
        }

        [[nodiscard]]
        const Symbol* symbol() const noexcept {
            return m_symbol;
        }

        bool operator==(const AddressLiteral &rhs) const noexcept {
            if (this == &rhs) {
                return true;
            }

            return m_symbol == rhs.m_symbol && m_displacement == rhs.m_displacement;
        }

    private:
        const Symbol* m_symbol;
        std::uint32_t m_displacement;
    };

    inline std::ostream &operator<<(std::ostream &os, const AddressLiteral &address) {
        return os << address.m_symbol->name();
    }
}
