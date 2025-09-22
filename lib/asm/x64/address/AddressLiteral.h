#pragma once

namespace aasm {
    class AddressLiteral final {
    public:
        constexpr explicit AddressLiteral(const Symbol* symbol, const std::int32_t displacement) noexcept:
            m_symbol(symbol),
            m_displacement(displacement) {}

        friend std::ostream &operator<<(std::ostream &os, const AddressLiteral &address);

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> encode(Buffer &buffer, const std::uint32_t modrm_pattern, std::int32_t offset_to_end) const {
            static constexpr std::uint8_t MODRM = 0x05; // ModR/M byte for direct addressing
            buffer.emit8((modrm_pattern & 0x7) << 3 | MODRM);
            buffer.emit32(INT32_MIN);

            RelType rel_type;
            switch (const auto bind = m_symbol->bind()) {
                case BindAttribute::EXTERNAL: rel_type = RelType::X86_64_PLT32; break;
                case BindAttribute::INTERNAL: rel_type = RelType::X86_64_PC32; break;
                default: die("Unsupported linkage type for AddressLiteral: {}", static_cast<std::uint8_t>(bind));
            }
            return Relocation(rel_type, buffer.size()-sizeof(std::int32_t), buffer.size()+offset_to_end-m_displacement, m_symbol);
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

        [[nodiscard]]
        std::int32_t offset() const noexcept {
            return m_displacement;
        }

        [[nodiscard]]
        AddressLiteral add_offset(const std::int32_t offset) const noexcept {
            const auto new_offset = static_cast<std::int64_t>(offset) + m_displacement;
            return AddressLiteral(m_symbol, aasm::checked_cast<std::int32_t>(new_offset));
        }

    private:
        const Symbol* m_symbol;
        std::int32_t m_displacement;
    };
}
