#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> MOVSXD = {0x63};

    class MovsxdRR final {
    public:
        explicit constexpr MovsxdRR(const std::uint8_t from_size, const std::uint8_t to_size, const GPReg& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_src(src),
            m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovsxdRR& movsxrr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            if (m_from_size != 4 && m_to_size != 8) {
                die("Invalid size for instruction: {}", m_from_size);
            }

            Encoder enc(buffer, {}, MOVSXD);
            return enc.encode_MR(m_to_size, m_from_size, m_dest, m_src);
        }

    private:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        GPReg m_src;
        GPReg m_dest;
    };

    inline std::ostream& operator<<(std::ostream &os, const MovsxdRR& movsxrr) {
        return os << "movs" << prefix_size(movsxrr.m_from_size)
            << prefix_size(movsxrr.m_to_size)
            << " %" << movsxrr.m_src.name(movsxrr.m_from_size)
            << ", %" << movsxrr.m_dest.name(movsxrr.m_to_size);
    }

    class MovsxdRM final {
    public:
        explicit constexpr MovsxdRM(const std::uint8_t from_size, const std::uint8_t to_size, const Address& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_dest(dest),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const MovsxdRM& movsxrm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            if (m_from_size != 4 && m_to_size != 8) {
                die("Invalid size for instruction: {}", m_from_size);
            }

            Encoder enc(buffer, {}, MOVSXD);
            return enc.encode_MR(m_to_size, m_from_size, m_dest, m_src);
        }

    private:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        GPReg m_dest;
        const Address m_src;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovsxdRM& movsxrm) {
        return os << "movs" << prefix_size(movsxrm.m_from_size)
            << prefix_size(movsxrm.m_to_size)
            << " " << movsxrm.m_src
            << ", %" << movsxrm.m_dest.name(movsxrm.m_to_size);
    }
}