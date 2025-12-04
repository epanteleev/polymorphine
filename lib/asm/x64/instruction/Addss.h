#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> ADDSS_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 1> ADDSD_PREFIX = {0xF2};
    static constexpr std::array<std::uint8_t, 2> ADDSD = {0x0F, 0x58};

    template<typename SRC, std::array<std::uint8_t, 1> PREFIX>
    class AddssA_Base {
    public:
        template<typename S = SRC>
        explicit constexpr AddssA_Base(S&& src, const XmmReg dst) noexcept:
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
        XmmReg m_dst;
    };

    class AddssRR final: public AddssA_Base<XmmReg, ADDSS_PREFIX> {
    public:
        explicit constexpr AddssRR(const XmmReg src, const XmmReg dst) noexcept:
            AddssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddssRR& rr);
    };

    class AddssRM final: public AddssA_Base<Address, ADDSS_PREFIX> {
    public:
        explicit constexpr AddssRM(const Address& src, const XmmReg dst) noexcept:
            AddssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddssRM& rr);
    };

    class AddsdRR final: public AddssA_Base<XmmReg, ADDSD_PREFIX> {
    public:
        explicit constexpr AddsdRR(const XmmReg src, const XmmReg dst) noexcept:
            AddssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddsdRR& rr);
    };

    class AddsdRM final: public AddssA_Base<Address, ADDSD_PREFIX> {
    public:
        explicit constexpr AddsdRM(const Address& src, const XmmReg dst) noexcept:
            AddssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const AddsdRM& rr);
    };
}