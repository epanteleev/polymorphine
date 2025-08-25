#pragma once

namespace aasm::details {
    class SubRR final {
    public:
        constexpr explicit SubRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream& os, const SubRR& subrr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SUB_RR = {0x29};
            static constexpr std::array<std::uint8_t, 1> SUB_RR_8 = {0x28};
            Encoder enc(buffer, SUB_RR_8, SUB_RR);
            enc.encode_MR(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const SubRR &subrr) {
        return os << "sub" << prefix_size(subrr.m_size) << " %"
                   << subrr.m_src.name(subrr.m_size) << ", %"
                   << subrr.m_dst.name(subrr.m_size);
    }

    class SubRI final {
    public:
        constexpr explicit SubRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SubRI &subrr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SUB_RI = {0x81};
            static constexpr std::array<std::uint8_t, 1> SUB_RI_8 = {0x80};
            Encoder enc(buffer, SUB_RI_8, SUB_RI);
            enc.encode_RI32(5, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        std::int32_t m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const SubRI &subrr) {
        return os << "sub" << prefix_size(subrr.m_size) << " $"
                   << subrr.m_src << ", %" << subrr.m_dst.name(subrr.m_size);
    }

    class SubMI final {
    public:
        constexpr explicit SubMI(std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SubMI &submi);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SUB_MI = {0x81};
            static constexpr std::array<std::uint8_t, 1> SUB_MI_8 = {0x80};
            Encoder enc(buffer, SUB_MI_8, SUB_MI);
            return enc.encode_MI32(5, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        std::int32_t m_src;
        Address m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const SubMI &submi) {
        return os << "sub" << prefix_size(submi.m_size) << " $"
                   << submi.m_src << ", " << submi.m_dst;
    }

    class SubRM final {
    public:
        explicit SubRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SubRM &subrm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SUB_RM = {0x2B};
            static constexpr std::array<std::uint8_t, 1> SUB_RM_8 = {0x2A};
            Encoder enc(buffer, SUB_RM_8, SUB_RM);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        Address m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const SubRM &subrm) {
        return os << "sub" << prefix_size(subrm.m_size) << " "
                   << subrm.m_src << ", %" << subrm.m_dst.name(subrm.m_size);
    }

    class SubMR final {
    public:
        constexpr explicit SubMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dest(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SubMR &submr);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SUB_MR = {0x29};
            static constexpr std::array<std::uint8_t, 1> SUB_MR_8 = {0x28};
            Encoder enc(buffer, SUB_MR_8, SUB_MR);
            return enc.encode_MR(m_size, m_src, m_dest);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        Address m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const SubMR &submr) {
        return os << "sub" << prefix_size(submr.m_size) << " %"
                   << submr.m_src.name(submr.m_size) << ", " << submr.m_dest;
    }
}
