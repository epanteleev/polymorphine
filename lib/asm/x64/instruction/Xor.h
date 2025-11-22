#pragma once

namespace aasm::details {
    template<typename DST>
    class XorRM_R {
    public:
        template<typename D = DST>
        constexpr XorRM_R(const std::uint8_t size, const GPReg src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR = {0x31};
            static constexpr std::array<std::uint8_t, 1> XOR_8 = {0x30};
            Encoder enc(buffer, XOR_8, XOR);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        GPReg m_src;
        DST m_dst;
    };

    class XorRR final: public XorRM_R<GPReg> {
    public:
        constexpr XorRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            XorRM_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorRR& xorrr);
    };

    class XorMR final: public XorRM_R<Address> {
    public:
        constexpr explicit XorMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            XorRM_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorMR& xormr);
    };

    template<typename DST>
    class XorRM_I {
    public:
        template<typename D = DST>
        explicit XorRM_I(const std::uint8_t size, const std::int32_t src, D&& dst) noexcept:
                    m_size(size),
                    m_src(src),
                    m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR = {0x81};
            static constexpr std::array<std::uint8_t, 1> XOR_8 = {0x80};
            Encoder enc(buffer, XOR_8, XOR);
            return enc.encode_MI32(6, m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        std::int32_t m_src;
        DST m_dst;
    };

    class XorRI final: public XorRM_I<GPReg> {
    public:
        explicit XorRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            XorRM_I(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorRI& xorri);
    };

    class XorMI final: public XorRM_I<Address> {
    public:
        constexpr explicit XorMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            XorRM_I(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorMI& xormi);
    };

    class XorRM final {
    public:
        constexpr explicit XorRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const XorRM& xorrm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR = {0x33};
            static constexpr std::array<std::uint8_t, 1> XOR_8 = {0x32};
            Encoder enc(buffer, XOR_8, XOR);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };
}