#pragma once

namespace aasm::details {
    class Jmp final {
    public:
        constexpr explicit Jmp(const Label& label) noexcept:
            m_label(label) {}

        friend std::ostream &operator<<(std::ostream &os, const Jmp &jmp);

        static constexpr std::uint8_t JMP = 0xE9;
        static constexpr std::uint8_t JMP_8 = 0xEB;

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer, const std::int32_t offset) const {
            if (std::in_range<std::int8_t>(offset)) {
                buffer.emit8(JMP_8);
                buffer.emit8(static_cast<std::int8_t>(offset) - 2);

            } else {
                buffer.emit8(JMP);
                buffer.emit32(offset - 5);
            }
        }

        template<CodeBuffer Buffer>
        constexpr void emit_unresolved32(Buffer& buffer) const {
            buffer.emit8(JMP);
            buffer.emit32(INT32_MAX);
        }

        [[nodiscard]]
        constexpr const Label& label() const noexcept {
            return m_label;
        }

    private:
        const Label m_label;
    };

    inline std::ostream &operator<<(std::ostream &os, const Jmp &jmp) {
        return os << "jmp " << jmp.m_label;
    }
}