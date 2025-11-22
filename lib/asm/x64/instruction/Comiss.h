#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 0> COMISS_PREFIX;
    static constexpr std::array<std::uint8_t, 2> COMISD = {0x0F, 0x2F};

    template<typename SRC>
    class ComissR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr ComissR_RM(S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, COMISS_PREFIX, COMISD);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class ComissRR final: public ComissR_RM<XmmReg> {
    public:
        explicit constexpr ComissRR(const XmmReg src, const XmmReg dst) noexcept:
            ComissR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const ComissRR& rr);
    };

    class ComissRM final: public ComissR_RM<Address> {
    public:
        explicit constexpr ComissRM(const Address& src, const XmmReg dst) noexcept:
            ComissR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const ComissRM& rr);
    };
}