#pragma once

namespace aasm::details {
    template<typename DST>
    class OrRM_Base {
    public:
        template<typename D = DST>
        constexpr OrRM_Base(const std::uint8_t size, const GPReg src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> OR = {0x09};
            static constexpr std::array<std::uint8_t, 1> OR_8 = {0x08};
            Encoder enc(buffer, OR_8, OR);
            return enc.encode_MR(m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        GPReg m_src;
        DST m_dst;
    };

    class OrRR final: public OrRM_Base<GPReg> {
    public:
        constexpr OrRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            OrRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const OrRR& oor);
    };

    class OrMR final: public OrRM_Base<Address> {
    public:
        constexpr explicit OrMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            OrRM_Base(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const OrMR& oor);
    };

    template<typename DST>
    class OrRI_R {
    public:
        template<typename D = DST>
        explicit constexpr OrRI_R(const std::uint8_t size, const std::int32_t src, D&& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(std::forward<D>(dst)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> ADD_RI = {0x81};
            static constexpr std::array<std::uint8_t, 1> ADD_RI_8 = {0x80};
            Encoder enc(buffer, ADD_RI_8, ADD_RI);
            return enc.encode_MI32(1, m_size, m_src, m_dst);
        }

    protected:
        std::uint8_t m_size;
        std::int32_t m_src;
        DST m_dst;
    };

    class OrRI final: public OrRI_R<GPReg> {
    public:
        explicit OrRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept:
            OrRI_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const OrRI& oor);
    };

    class OrMI final: public OrRI_R<Address> {
    public:
        constexpr explicit OrMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            OrRI_R(size, src, dst) {}

        friend std::ostream& operator<<(std::ostream &os, const OrMI& oor);
    };

    class OrRM final {
    public:
        constexpr explicit OrRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const OrRM& oor);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> OR_RM = {0x0B};
            static constexpr std::array<std::uint8_t, 1> OR_RM_8 = {0x0A};
            Encoder enc(buffer, OR_RM_8, OR_RM);
            return enc.encode_RM(m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };
}