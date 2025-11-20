#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> COMISD_PREFIX = {0x66};

    template<typename SRC>
    class ComisdR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr ComisdR_RM(S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, COMISD_PREFIX, COMISD);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class ComisdRR final: public ComisdR_RM<XmmReg> {
    public:
        explicit constexpr ComisdRR(const XmmReg src, const XmmReg dst) noexcept:
            ComisdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const ComisdRR& rr) {
            return os << "comisd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }
    };

    class ComisdRM final: public ComisdR_RM<Address> {
    public:
        explicit constexpr ComisdRM(const Address& src, const XmmReg dst) noexcept:
            ComisdR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const ComisdRM& rr) {
            return os << "comisd " << rr.m_src << ", %" << rr.m_dst.name(16);
        }
    };
}