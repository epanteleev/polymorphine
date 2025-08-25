#pragma once

namespace aasm::details {
    class XorRR final {
    public:
        constexpr XorRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorRR& xorrr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR_RR = {0x31};
            static constexpr std::array<std::uint8_t, 1> XOR_RR_8 = {0x30};
            Encoder enc(buffer, XOR_RR_8, XOR_RR);
            enc.encode_MR(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const XorRR &xorrr) {
        return os << "xor" << prefix_size(xorrr.m_size) << " %" << xorrr.m_src.name(xorrr.m_size) << ", %" << xorrr.m_dst.name(xorrr.m_size);
    }

    class XorRI final {
    public:
        explicit XorRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorRI& xorri);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR_RI = {0x81};
            static constexpr std::array<std::uint8_t, 1> XOR_RI_8 = {0x80};
            Encoder enc(buffer, XOR_RI_8, XOR_RI);
            enc.encode_RI32(6, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        std::int32_t m_src;
        GPReg m_dst;
    };

    inline std::ostream& operator<<(std::ostream &os, const XorRI &xorri) {
        return os << "xor" << prefix_size(xorri.m_size) << " $" << xorri.m_src << ", %" << xorri.m_dst.name(xorri.m_size);
    }

    class XorMI final {
    public:
        constexpr explicit XorMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorMI& xormi);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR_MI = {0x81};
            static constexpr std::array<std::uint8_t, 1> XOR_MI_8 = {0x80};
            Encoder enc(buffer, XOR_MI_8, XOR_MI);
            return enc.encode_MI32(6, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        std::int32_t m_src;
        Address m_dst;
    };

    inline std::ostream& operator<<(std::ostream &os, const XorMI &xormi) {
        return os << "xor" << prefix_size(xormi.m_size) << " $" << xormi.m_src << ", " << xormi.m_dst;
    }

    class XorRM final {
    public:
        constexpr explicit XorRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorRM& xorrm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR_RM = {0x33};
            static constexpr std::array<std::uint8_t, 1> XOR_RM_8 = {0x32};
            Encoder enc(buffer, XOR_RM_8, XOR_RM);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        Address m_src;
        GPReg m_dst;
    };

    inline std::ostream& operator<<(std::ostream &os, const XorRM &xorrm) {
        return os << "xor" << prefix_size(xorrm.m_size) << ' ' << xorrm.m_src << ", %" << xorrm.m_dst.name(xorrm.m_size);
    }

    class XorMR final {
    public:
        constexpr explicit XorMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const XorMR& xormr);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> XOR_MR = {0x31};
            static constexpr std::array<std::uint8_t, 1> XOR_MR_8 = {0x30};
            Encoder enc(buffer, XOR_MR_8, XOR_MR);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        Address m_dst;
    };

    inline std::ostream& operator<<(std::ostream &os, const XorMR &xormr) {
        return os << "xor" << prefix_size(xormr.m_size) << " %" << xormr.m_src.name(xormr.m_size) << ", " << xormr.m_dst;
    }
}