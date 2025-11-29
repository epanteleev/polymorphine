#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> MOVSS_RM_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 2> MOVSS_RR = {0x0F, 0x10};
    static constexpr std::array<std::uint8_t, 2> MOVSS_MR = {0x0F, 0x11};

    template<typename SRC>
    class MovssR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr MovssR_RM(S&& src, XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, MOVSS_RM_PREFIX, MOVSS_RR);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class MovssRR final: public MovssR_RM<XmmReg> {
    public:
        explicit constexpr MovssRR(const XmmReg src, const XmmReg dst) noexcept:
            MovssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const MovssRR& rr);
    };

    class MovssRM final: public MovssR_RM<Address> {
    public:
        explicit constexpr MovssRM(const Address& src, const XmmReg dst) noexcept:
            MovssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const MovssRM& rr);
    };

    class MovssMR final {
    public:
        explicit constexpr MovssMR(const XmmReg src, const Address& dst) noexcept:
            m_src(src),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, MOVSS_RM_PREFIX, MOVSS_MR);
            return encoder.encode_C(m_src, m_dst);
        }

        friend std::ostream& operator<<(std::ostream& os, const MovssMR& rr);

    protected:
        XmmReg m_src;
        Address m_dst;
    };
}