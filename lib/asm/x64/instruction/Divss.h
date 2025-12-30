#pragma once


namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> DIVSS_PREFIX = {0xF3};
    static constexpr std::array<std::uint8_t, 1> DIVSD_PREFIX = {0xF2};
    static constexpr std::array<std::uint8_t, 2> DIVSD = {0x0F, 0x5E};

    template<typename SRC, std::array<std::uint8_t, 1> PREFIX>
    class DivssA_Base {
    public:
        template<typename S = SRC>
        explicit constexpr DivssA_Base(S&& src, const XmmReg dst) noexcept:
            m_src(std::forward<S>(src)),
            m_dst(dst) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            SSEEncoder encoder(buffer, PREFIX, DIVSD);
            return encoder.encode_A(m_src, m_dst);
        }

    protected:
        SRC m_src;
        XmmReg m_dst;
    };

    class DivssRR final: public DivssA_Base<XmmReg, DIVSS_PREFIX> {
    public:
        explicit constexpr DivssRR(const XmmReg src, const XmmReg dst) noexcept:
            DivssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const DivssRR& rr);
    };

    class DivssRM final: public DivssA_Base<Address, DIVSS_PREFIX> {
    public:
        explicit constexpr DivssRM(const Address& src, const XmmReg dst) noexcept:
            DivssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const DivssRM& rr);
    };

    class DivsdRR final: public DivssA_Base<XmmReg, DIVSD_PREFIX> {
    public:
        explicit constexpr DivsdRR(const XmmReg src, const XmmReg dst) noexcept:
            DivssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const DivsdRR& rr);
    };

    class DivsdRM final: public DivssA_Base<Address, DIVSD_PREFIX> {
    public:
        explicit constexpr DivsdRM(const Address& src, const XmmReg dst) noexcept:
            DivssA_Base(src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const DivsdRM& rr);
    };
}