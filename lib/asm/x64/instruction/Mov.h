#pragma once


namespace aasm::details {
    class MovRR final {
    public:
        explicit constexpr MovRR(std::uint8_t size, const GPReg& src, const GPReg& dest) noexcept:
            m_size(size), m_src(src), m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovRR& movrr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> MOV_RR = {0x89};
            static constexpr std::array<std::uint8_t, 1> MOV_RR_8 = {0x88};
            Encoder enc(buffer, MOV_RR_8, MOV_RR);
            enc.encode_MR(m_size, m_src, m_dest);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovRR &movrr) {
        return os << "mov" << prefix_size(movrr.m_size) << " %" << movrr.m_src.name(movrr.m_size) << ", %" << movrr.m_dest.name(movrr.m_size);
    }

    class MovRI final {
    public:
        constexpr MovRI(std::uint8_t size, const std::int64_t src, const GPReg& dest) noexcept:
            m_size(size), m_src(src), m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovRI &movri);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> MOV_RI_8 = {0xB0};
            static constexpr std::array<std::uint8_t, 1> MOV_RI = {0xB8};
            Encoder enc(buffer, MOV_RI_8, MOV_RI);
            enc.encode_RI64(m_size, m_src, m_dest);
        }

    private:
        std::uint8_t m_size;
        std::int64_t m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovRI &movri) {
        if (movri.m_size == 8) {
            os << "movabs";
        } else {
            os << "mov";
        }

        return os << prefix_size(movri.m_size) << " $" << movri.m_src << ", %" << movri.m_dest.name(movri.m_size);
    }

    class MovMR final {
    public:
        constexpr MovMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dest(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const MovMR &movmr);

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> MOV_MR = {0x89};
            static constexpr std::array<std::uint8_t, 1> MOV_MR_8 = {0x88};
            Encoder enc(buffer, MOV_MR_8, MOV_MR);
            return enc.encode_MR(m_size, m_src, m_dest);
        }

    private:
        std::uint8_t m_size;
        const GPReg m_src;
        const Address m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovMR &movmr) {
        return os << "mov" << prefix_size(movmr.m_size) << " %" << movmr.m_src.name(movmr.m_size) << ", " << movmr.m_dest;
    }

    class MovRM final {
    public:
        constexpr MovRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_src(src),
            m_size(size),
            m_dest(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const MovRM &movrm);

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> MOV_RM = {0x8B};
            static constexpr std::array<std::uint8_t, 1> MOV_RM_8 = {0x8A};
            Encoder enc(buffer, MOV_RM_8, MOV_RM);
            return enc.encode_RM(m_size, m_src, m_dest);
        }

    private:
        const Address m_src;
        std::uint8_t m_size;
        const GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovRM &movrm) {
        return os << "mov" << prefix_size(movrm.m_size) << " " << movrm.m_src << ", %" << movrm.m_dest.name(movrm.m_size);
    }

    class MovMI final {
    public:
        constexpr MovMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            m_src(src),
            m_dest(dst),
            m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const MovMI &movrm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> MOV_MI = {0xC7};
            static constexpr std::array<std::uint8_t, 1> MOV_MI_8 = {0xC6};
            Encoder enc(buffer, MOV_MI_8, MOV_MI);
            return enc.encode_MI32(0, m_size, m_src, m_dest);
        }

    private:
        const std::int32_t m_src;
        const Address m_dest;
        std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovMI &movrm) {
        return os << "mov" << prefix_size(movrm.m_size) << " $" << movrm.m_src << ", " << movrm.m_dest;
    }
}
