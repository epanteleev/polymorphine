#pragma once

namespace aasm::details {
    template<typename DST>
    class AndRM_Base {
    public:
        template<typename D = DST>
        constexpr AndRM_Base(const std::uint8_t size, const GPReg src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> AND = {0x20};
            static constexpr std::array<std::uint8_t, 1> AND_8 = {0x21};
            Encoder enc(buffer, AND_8, AND);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        GPReg m_src;
        DST m_dst;
    };

    class AndRR final: public AndRM_Base<GPReg> {
    public:
        constexpr AndRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            AndRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AndRR& aand);
    };

    class AndMR final: public AndRM_Base<Address> {
    public:
        constexpr explicit AndMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            AndRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AndMR& aand);
    };

    template<typename DST>
    class AndRI_R {
    public:
        template<typename D = DST>
        explicit constexpr AndRI_R(const std::uint8_t size, const std::int32_t src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_RI = {0x81};
            static constexpr std::array<std::uint8_t, 1> ADD_RI_8 = {0x80};
            Encoder enc(buffer, ADD_RI_8, ADD_RI);
            return enc.encode_MI32(4, m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        std::int32_t m_src;
        DST m_dst;
    };

    class AndRI final: public AndRI_R<GPReg> {
    public:
        explicit AndRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            AndRI_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AndRI& aand);
    };

    class AndMI final: public AndRI_R<Address> {
    public:
        constexpr explicit AndMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            AndRI_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AndMI& aand);
    };

    class AndRM final {
    public:
        constexpr explicit AndRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const AndRM& aand);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_RM = {0x22};
            static constexpr std::array<std::uint8_t, 1> ADD_RM_8 = {0x23};
            Encoder enc(buffer, ADD_RM_8, ADD_RM);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };
}