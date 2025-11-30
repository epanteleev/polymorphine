#pragma once


namespace aasm::details {
    template<typename SRC>
    class XorpdR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr XorpdR_RM(S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, DB_PREFIX, XORPS);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class XorpdRR final: public XorpdR_RM<XmmReg> {
    public:
        explicit constexpr XorpdRR(const XmmReg src, const XmmReg dst) noexcept:
            XorpdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const XorpdRR& rr);
    };

    class XorpdRM final: public XorpdR_RM<Address> {
    public:
        explicit constexpr XorpdRM(const Address& src, const XmmReg dst) noexcept:
            XorpdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const XorpdRM& rr);
    };
}