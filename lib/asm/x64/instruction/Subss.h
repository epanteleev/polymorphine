#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> SUBSS_PREFIX = {0xF2};
    static constexpr std::array<std::uint8_t, 1> SUBSD_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 2> SUBSD = {0x0F, 0x58};

    template<typename SRC, std::array<std::uint8_t, 1> PREFIX>
    class SubssR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr SubssR_RM(S&& src, XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, PREFIX, SUBSD);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class SubssRR final: public SubssR_RM<XmmReg, SUBSS_PREFIX> {
    public:
        explicit constexpr SubssRR(const XmmReg src, const XmmReg dst) noexcept:
            SubssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const SubssRR& rr) {
            return os << "subss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }
    };

    class SubssRM final: public SubssR_RM<Address, SUBSS_PREFIX> {
    public:
        explicit constexpr SubssRM(const Address& src, const XmmReg dst) noexcept:
            SubssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const SubssRM& rr) {
            return os << "subss " << rr.m_src << ", %" << rr.m_dst.name(16);
        }
    };

    class SubsdRR final: public SubssR_RM<XmmReg, SUBSD_PREFIX> {
    public:
        explicit constexpr SubsdRR(const XmmReg src, const XmmReg dst) noexcept:
            SubssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const SubsdRR& rr) {
            return os << "subsd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }
    };

    class SubsdRM final: public SubssR_RM<Address, SUBSS_PREFIX> {
    public:
        explicit constexpr SubsdRM(const Address& src, const XmmReg dst) noexcept:
            SubssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const SubsdRM& rr) {
            return os << "subsd " << rr.m_src << ", %" << rr.m_dst.name(16);
        }
    };
}