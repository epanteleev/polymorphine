#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 2> XORPS = {0x0F, 0x57};

    template<typename SRC>
    class XorpsR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr XorpsR_RM(S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, SP_PREFIX, XORPS);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class XorpsRR final: public XorpsR_RM<XmmReg> {
    public:
        explicit constexpr XorpsRR(const XmmReg src, const XmmReg dst) noexcept:
            XorpsR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const XorpsRR& rr);
    };

    class XorpsRM final: public XorpsR_RM<Address> {
    public:
        explicit constexpr XorpsRM(const Address& src, const XmmReg dst) noexcept:
            XorpsR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const XorpsRM& rr);
    };
}