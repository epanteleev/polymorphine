#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> CVTSI2SS_RM_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 2> CVTSI2SS = {0x0F, 0x2A};

    template<typename SRC>
    class Cvtsi2ssR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr Cvtsi2ssR_RM(const std::uint8_t size, S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst),
            m_size(size) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, CVTSI2SS_RM_PREFIX, CVTSI2SS);
            switch (m_size) {
                case 8: [[fallthrough]];
                case 4: return encoder.encode_A(m_size, m_src, m_dst);
                default: die("Unsupported size: {}", m_size);
            }
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
        std::uint8_t m_size;
    };

    class Cvtsi2ssRR final: public Cvtsi2ssR_RM<GPReg> {
    public:
        explicit constexpr Cvtsi2ssRR(const std::uint8_t size, const GPReg src, const XmmReg dst) noexcept:
            Cvtsi2ssR_RM(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtsi2ssRR& rr);
    };

    class Cvtsi2ssRM final: public Cvtsi2ssR_RM<Address> {
    public:
        explicit constexpr Cvtsi2ssRM(const std::uint8_t size, const Address& src, const XmmReg dst) noexcept:
            Cvtsi2ssR_RM(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const Cvtsi2ssRM& rr);
    };
}