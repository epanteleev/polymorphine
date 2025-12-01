#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> CVTSD2SI_RM_PREFIX = {0xF2};

    template<typename SRC>
    class Cvtsd2siR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr Cvtsd2siR_RM(const std::uint8_t to_size, S&& src, const GPReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst),
            m_size(to_size) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, CVTSD2SI_RM_PREFIX, CVTSS2SI);
            switch (m_size) {
                case 8: [[fallthrough]];
                case 4: return encoder.encode_A(m_size, m_src, m_dst);
                default: die("Unsupported size: {}", m_size);
            }
        }

    protected:
        SRC m_src;
        GPReg m_dst;
        std::uint8_t m_size;
    };

    class Cvtsd2siRR final: public Cvtsd2siR_RM<XmmReg> {
    public:
        explicit constexpr Cvtsd2siRR(const std::uint8_t to_size, const XmmReg src, const GPReg dst) noexcept:
            Cvtsd2siR_RM(to_size, src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtsd2siRR& rr);
    };

    class Cvtsd2siRM final: public Cvtsd2siR_RM<Address> {
    public:
        explicit constexpr Cvtsd2siRM(const std::uint8_t to_size, const Address& src, const GPReg dst) noexcept:
            Cvtsd2siR_RM(to_size, src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtsd2siRM& rr);
    };
}