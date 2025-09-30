#pragma once

namespace aasm::details {
    template<typename DST>
    class SubRM_R {
    public:
        template<typename D = DST>
        constexpr explicit SubRM_R(const std::uint8_t size, const GPReg src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SUB_RR = {0x29};
            static constexpr std::array<std::uint8_t, 1> SUB_RR_8 = {0x28};
            Encoder enc(buffer, SUB_RR_8, SUB_RR);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        GPReg m_src;
        DST m_dst;
    };

    class SubRR final: public SubRM_R<GPReg> {
    public:
        constexpr explicit SubRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            SubRM_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream& os, const SubRR& subrr) {
            return os << "sub" << prefix_size(subrr.m_size) << " %"
                   << subrr.m_src.name(subrr.m_size) << ", %"
                   << subrr.m_dst.name(subrr.m_size);
        }
    };

    class SubMR final: public SubRM_R<Address> {
    public:
        constexpr explicit SubMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            SubRM_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SubMR &submr) {
            return os << "sub" << prefix_size(submr.m_size) << " %"
                   << submr.m_src.name(submr.m_size) << ", " << submr.m_dst;
        }
    };

    template<typename DST>
    class SubRM_I {
    public:
        template<typename D = DST>
        constexpr explicit SubRM_I(const std::uint8_t size, const std::int32_t src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> SUB = {0x81};
            static constexpr std::array<std::uint8_t, 1> SUB_8 = {0x80};
            Encoder enc(buffer, SUB_8, SUB);
            return enc.encode_MI32(5, m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        std::int32_t m_src;
        DST m_dst;
    };

    class SubRI final: public SubRM_I<GPReg> {
    public:
        constexpr explicit SubRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            SubRM_I(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SubRI &subrr) {
            return os << "sub" << prefix_size(subrr.m_size) << " $"
                   << subrr.m_src << ", %" << subrr.m_dst.name(subrr.m_size);
        }
    };

    class SubMI final: public SubRM_I<Address> {
    public:
        constexpr explicit SubMI(std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            SubRM_I(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const SubMI &submi) {
            return os << "sub" << prefix_size(submi.m_size) << " $"
                   << submi.m_src << ", " << submi.m_dst;
        }
    };

    class SubRM final {
    public:
        explicit SubRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const SubRM &subrm) {
            return os << "sub" << prefix_size(subrm.m_size) << " " << subrm.m_src << ", %" << subrm.m_dst.name(subrm.m_size);
        }

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
        GPReg m_dst;
        Address m_src;
    };
}