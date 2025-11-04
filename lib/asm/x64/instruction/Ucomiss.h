#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 0> UCOMISS_PREFIX;
    static constexpr std::array<std::uint8_t, 1> UCOMISD_PREFIX = {0x66};
    static constexpr std::array<std::uint8_t, 2> UCOMISD = {0x0F, 0x2E};

    template<typename SRC>
    class UcomissR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr UcomissR_RM(S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, UCOMISS_PREFIX, UCOMISD);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class UcomissRR final: public UcomissR_RM<XmmReg> {
    public:
        explicit constexpr UcomissRR(const XmmReg src, const XmmReg dst) noexcept:
            UcomissR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const UcomissRR& rr) {
            return os << "ucomiss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }
    };

    class UcomissRM final: public UcomissR_RM<Address> {
    public:
        explicit constexpr UcomissRM(const Address& src, const XmmReg dst) noexcept:
            UcomissR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const UcomissRM& rr) {
            return os << "ucomiss " << rr.m_src << ", %" << rr.m_dst.name(16);
        }
    };
}