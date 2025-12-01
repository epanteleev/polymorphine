#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> CVTSS2SI_RM_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 2> CVTSS2SI = {0x0F, 0x2C};

    template<typename SRC>
    class Cvtss2siR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr Cvtss2siR_RM(const std::uint8_t size, S&& src, const GPReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst),
            m_size(size) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, CVTSS2SI_RM_PREFIX, CVTSS2SI);
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

    class Cvtss2siRR final: public Cvtss2siR_RM<XmmReg> {
    public:
        explicit constexpr Cvtss2siRR(const std::uint8_t size, const XmmReg src, const GPReg dst) noexcept:
            Cvtss2siR_RM(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtss2siRR& rr);
    };

    class Cvtss2siRM final: public Cvtss2siR_RM<Address> {
    public:
        explicit constexpr Cvtss2siRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            Cvtss2siR_RM(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtss2siRM& rr);
    };
}