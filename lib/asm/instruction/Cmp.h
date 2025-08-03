#pragma once

namespace aasm::details {
    class CmpRR final {
    public:
        constexpr CmpRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            m_size(size), m_src(src), m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRR& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMP_RR = 0x39;
            static constexpr std::uint8_t CMP_RR_8 = 0x38;
            encode_RR<CMP_RR_8, CMP_RR>(buffer, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpRR &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " %"
                  << cmp.m_src.name(cmp.m_size) << ", %"
                  << cmp.m_dst.name(cmp.m_size);
    }

    class CmpRI final {
    public:
        constexpr CmpRI(const std::uint8_t size, const std::int32_t imm, const GPReg dst) noexcept:
            m_imm(imm),
            m_size(size),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRI& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            encode_RI32<CMP_RI_8, CMP_RI, 0x38>(buffer, m_size, m_imm, m_dst);
        }

    private:
        static constexpr std::uint8_t CMP_RI = 0x81;
        static constexpr std::uint8_t CMP_RI_8 = 0x80;

        std::int32_t m_imm;
        std::uint8_t m_size;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpRI &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " $"
                  << cmp.m_imm << ", %" << cmp.m_dst.name(cmp.m_size);
    }

    class CmpRM final {
    public:
        constexpr CmpRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRM& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMP_MR = 0x3B;
            static constexpr std::uint8_t CMP_MR_8 = 0x3A;
            encode_RM<CMP_MR_8, CMP_MR>(buffer, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpRM &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << ' '
                  << cmp.m_src << ", %" << cmp.m_dst.name(cmp.m_size);
    }

    class CmpMR final {
    public:
        constexpr CmpMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpMR& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMP_MR = 0x3B;
            static constexpr std::uint8_t CMP_MR_8 = 0x3A;
            encode_MR<CMP_MR_8, CMP_MR>(buffer, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        Address m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpMR &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " %" << cmp.m_src.name(cmp.m_size) << ", " << cmp.m_dst;
    }

    class CmpMI final {
    public:
        constexpr CmpMI(const std::uint8_t size, const std::int32_t imm, const Address& second) noexcept:
            m_second(second),
            m_imm(imm),
            m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpMI& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMP_MI = 0x81;
            static constexpr std::uint8_t CMP_MI_8 = 0x80;
            encode_MI32<CMP_MI_8, CMP_MI, 0x38>(buffer, m_size, m_imm, m_second);
        }

    private:
        Address m_second;
        std::int32_t m_imm;
        std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpMI &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size)
            << " $" << cmp.m_imm << ", " << cmp.m_second;
    }
}
