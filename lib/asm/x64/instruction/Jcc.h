#pragma once

namespace aasm::details {
    class Jcc final {
    public:
        explicit constexpr Jcc(const CondType type, const Label& label) noexcept:
            m_type(type),
            m_label(label) {}

        friend std::ostream& operator<<(std::ostream &os, const Jcc& jcc);

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer, const std::int32_t offset) const {
            if (std::in_range<std::int8_t>(offset)) {
                buffer.emit8(static_cast<std::uint8_t>(m_type) | JCC_REL8);
                buffer.emit8(static_cast<std::int8_t>(offset) - 2);

            } else {
                buffer.emit8(JCC_REL32);
                buffer.emit8(static_cast<std::uint8_t>(m_type) | 0x80);
                buffer.emit32(offset - 6);
            }

            return std::nullopt;
        }

        template<CodeBuffer Buffer>
        constexpr void emit_unresolved32(Buffer& buffer) const {
            buffer.emit8(JCC_REL32);
            buffer.emit8(static_cast<std::uint8_t>(m_type) | 0x80);
            buffer.emit32(INT32_MAX);
        }

        [[nodiscard]]
        constexpr const Label& label() const noexcept {
            return m_label;
        }

    private:
        static constexpr std::uint8_t JCC_REL8 = 0x70;
        static constexpr std::uint8_t JCC_REL32 = 0x0F;

        CondType m_type;
        Label m_label;
    };

    inline std::ostream & operator<<(std::ostream &os, const Jcc &jcc) {
        return os << "j" << jcc.m_type << ' ' << jcc.m_label;
    }
}
