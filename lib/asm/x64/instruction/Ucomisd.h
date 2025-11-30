#pragma once

namespace aasm::details {
    template<typename SRC>
    class UcomisdR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr UcomisdR_RM(S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, DB_PREFIX, UCOMISD);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class UcomisdRR final: public UcomisdR_RM<XmmReg> {
    public:
        explicit constexpr UcomisdRR(const XmmReg src, const XmmReg dst) noexcept:
            UcomisdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const UcomisdRR& rr);
    };

    class UcomisdRM final: public UcomisdR_RM<Address> {
    public:
        explicit constexpr UcomisdRM(const Address& src, const XmmReg dst) noexcept:
            UcomisdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const UcomisdRM& rr);
    };
}