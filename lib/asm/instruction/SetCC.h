#pragma once

namespace aasm {
    class SetCCR final {
    public:
        explicit constexpr SetCCR(CondType cond, GPReg reg) noexcept:
            m_cond(cond),
            m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream& os, const SetCCR& set);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            const auto rex = constants::REX | B(m_reg);
            if (rex != constants::REX && !is_special_byte_reg(m_reg)) {
                buffer.emit8(rex);
            }
            buffer.emit8(0x0F);
            buffer.emit8(static_cast<std::uint8_t>(m_cond) | 0x90);
        }

        [[nodiscard]]
        constexpr auto reg() const noexcept {
            return m_reg;
        }

        [[nodiscard]]
        constexpr auto cond() const noexcept {
            return m_cond;
        }

    private:
        CondType m_cond;
        GPReg m_reg;
    };

    inline std::ostream & operator<<(std::ostream &os, const SetCCR &set) {
        return os << "set" << set.m_cond;
    }
}
