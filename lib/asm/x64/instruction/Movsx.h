#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 2> MOVSX_RR = {0x0F, 0xBF};
    static constexpr std::array<std::uint8_t, 2> MOVSX_RR_8 = {0x0F, 0xBE};

    class MovsxRR final {
    public:
        explicit constexpr MovsxRR(const std::uint8_t from_size, const std::uint8_t to_size, const GPReg& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_src(src),
            m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovsxRR& movsxrr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            Encoder enc(buffer, MOVSX_RR_8, MOVSX_RR);
            switch (m_from_size) {
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 4: return enc.encode_MR(m_to_size, m_from_size, m_dest, m_src);
                default: die("Invalid size for instruction: {}", m_from_size);
            }
        }

    private:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        GPReg m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovsxRR &movsxrr) {
        return os << "movs" << prefix_size(movsxrr.m_from_size)
            << prefix_size(movsxrr.m_to_size)
            << " %" << movsxrr.m_src.name(movsxrr.m_from_size)
            << ", %" << movsxrr.m_dest.name(movsxrr.m_to_size);
    }

    class MovsxRM final {
    public:
        explicit constexpr MovsxRM(const std::uint8_t from_size, const std::uint8_t to_size, const Address& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_src(src),
            m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovsxRM& movsxrm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            Encoder enc(buffer, MOVSX_RR_8, MOVSX_RR);
            switch (m_from_size) {
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 4: return enc.encode_MR(m_to_size, m_from_size, m_dest, m_src);
                default: die("Invalid size for instruction: {}", m_from_size);
            }
        }

    private:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        Address m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovsxRM &movsxrm) {
        return os << "movs" << prefix_size(movsxrm.m_from_size)
            << prefix_size(movsxrm.m_to_size)
            << " " << movsxrm.m_src
            << ", %" << movsxrm.m_dest.name(movsxrm.m_to_size);
    }
}
