#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> MOVSD_RM_PREFIX = {0xF2};
    static constexpr std::array<std::uint8_t, 2> MOVSD_RR = {0x0F, 0x10};

    template<typename SRC>
    class MovsdR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr MovsdR_RM(S&& src, XmmRegister dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, MOVSD_RM_PREFIX, MOVSD_RR);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmRegister m_dst;
    };

    class MovsdRR final: public MovsdR_RM<XmmRegister> {
    public:
        explicit constexpr MovsdRR(const XmmRegister src, const XmmRegister dst) noexcept:
            MovsdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const MovsdRR& rr) {
            return os << "movsd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }
    };

    class MovsdRM final: public MovsdR_RM<Address> {
    public:
        explicit constexpr MovsdRM(const Address& src, const XmmRegister dst) noexcept:
            MovsdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const MovsdRM& rr) {
            return os << "movsd " << rr.m_src << ", %" << rr.m_dst.name(16);
        }
    };
}