#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> MOVSS_RM_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 2> MOVSS_RR = {0x0F, 0x10};

    class MovssRR final {
    public:
        explicit constexpr MovssRR(const XmmRegister src, const XmmRegister dst) noexcept:
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream& os, const MovssRR& rr) {
            return os << "movss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, MOVSS_RM_PREFIX, MOVSS_RR);
            encoder.encode_A(m_src, m_dst);
        }

    private:
        XmmRegister m_src;
        XmmRegister m_dst;
    };
}