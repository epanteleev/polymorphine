#pragma once

namespace aasm::details {
    class SetCCR final {
    public:
        explicit constexpr SetCCR(const CondType cond, const GPReg reg) noexcept:
            m_cond(cond),
            m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream& os, const SetCCR& set) {
            return os << "set" << set.m_cond << " %" << set.m_reg.name(1);
        }

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            EncodeUtils::emit_op_prologue(buffer, 1, m_reg);
            buffer.emit8(0x0F);
            buffer.emit8(0x90 | static_cast<std::uint8_t>(m_cond));
            buffer.emit8(m_reg.encode() | 0xC0);
            return std::nullopt;
        }

    private:
        CondType m_cond;
        GPReg m_reg;
    };
}
