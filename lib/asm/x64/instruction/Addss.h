#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> ADDSS_PREFIX = {0xF2};
    static constexpr std::array<std::uint8_t, 1> ADDSD_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 2> ADDSD = {0x0F, 0x58};

    template<typename SRC, std::array<std::uint8_t, 1> PREFIX>
    class AddssR_RM {
    public:
        template<typename S = SRC>
        explicit constexpr AddssR_RM(S&& src, XmmRegister dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, PREFIX, ADDSD);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmRegister m_dst;
    };

    class AddssRR final: public AddssR_RM<XmmRegister, ADDSS_PREFIX> {
    public:
        explicit constexpr AddssRR(const XmmRegister src, const XmmRegister dst) noexcept:
            AddssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddssRR& rr) {
            return os << "addss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }
    };

    class AddssRM final: public AddssR_RM<Address, ADDSS_PREFIX> {
    public:
        explicit constexpr AddssRM(const Address& src, const XmmRegister dst) noexcept:
            AddssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddssRM& rr) {
            return os << "addss " << rr.m_src << ", %" << rr.m_dst.name(16);
        }
    };

    class AddsdRR final: public AddssR_RM<XmmRegister, ADDSD_PREFIX> {
    public:
        explicit constexpr AddsdRR(const XmmRegister src, const XmmRegister dst) noexcept:
            AddssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddsdRR& rr) {
            return os << "addsd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
        }
    };

    class AddsdRM final: public AddssR_RM<Address, ADDSS_PREFIX> {
    public:
        explicit constexpr AddsdRM(const Address& src, const XmmRegister dst) noexcept:
            AddssR_RM(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddsdRM& rr) {
            return os << "addsd " << rr.m_src << ", %" << rr.m_dst.name(16);
        }
    };
}