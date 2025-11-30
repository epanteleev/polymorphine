#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> CVTSD2SI_RM_PREFIX = {0xF2};

    template<typename SRC>
    class Cvtsd2siR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr Cvtsd2siR_RM(S&& src, const GPReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, CVTSS2SI_RM_PREFIX, CVTSS2SI);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        GPReg m_dst;
    };

    class Cvtsd2siRR final: public Cvtsd2siR_RM<XmmReg> {
    public:
        explicit constexpr Cvtsd2siRR(const XmmReg src, const GPReg dst) noexcept:
            Cvtsd2siR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtsd2siRR& rr);
    };

    class Cvtsd2siRM final: public Cvtss2siR_RM<Address> {
    public:
        explicit constexpr Cvtsd2siRM(const Address& src, const GPReg dst) noexcept:
            Cvtss2siR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtsd2siRM& rr);
    };
}