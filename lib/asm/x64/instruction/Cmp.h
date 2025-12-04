#pragma once

namespace aasm::details {
    template<typename DST>
    class CmpRM_Base {
    public:
        template<typename D = DST>
        constexpr CmpRM_Base(const std::uint8_t size, const GPReg src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> CMP_RR = {0x39};
            static constexpr std::array<std::uint8_t, 1> CMP_RR_8 = {0x38};
            Encoder enc(buffer, CMP_RR_8, CMP_RR);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        GPReg m_src;
        DST m_dst;
    };

    class CmpRR final: public CmpRM_Base<GPReg> {
    public:
        constexpr CmpRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            CmpRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRR& cmp);
    };

    class CmpMR final: public CmpRM_Base<Address> {
    public:
        constexpr CmpMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            CmpRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpMR& cmp);
    };

    template<typename DST>
    class CmpRM_I {
    public:
        template<typename D = DST>
        constexpr CmpRM_I(const std::uint8_t size, const std::int32_t imm, D&& dst) noexcept:
            m_imm(imm),
            m_size(size),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> CMP_RI = {0x81};
            static constexpr std::array<std::uint8_t, 1> CMP_RI_8 = {0x80};
            Encoder enc(buffer, CMP_RI_8, CMP_RI);
            return enc.encode_MI32(7, m_size, m_imm, m_dst);
        }

    protected:
        std::int32_t m_imm;
        std::uint8_t m_size;
        DST m_dst;
    };

    class CmpRI final: public CmpRM_I<GPReg> {
    public:
        constexpr CmpRI(const std::uint8_t size, const std::int32_t imm, const GPReg dst) noexcept:
            CmpRM_I(size, imm, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRI& cmp);
    };

    class CmpMI final: public CmpRM_I<Address> {
    public:
        constexpr CmpMI(const std::uint8_t size, const std::int32_t imm, const Address& second) noexcept:
            CmpRM_I(size, imm, second) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpMI& cmp);
    };

    class CmpRM final {
    public:
        constexpr CmpRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRM& cmp);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> CMP_MR = {0x3B};
            static constexpr std::array<std::uint8_t, 1> CMP_MR_8 = {0x3A};
            Encoder enc(buffer, CMP_MR_8, CMP_MR);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };
}