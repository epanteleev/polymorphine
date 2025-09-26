#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 2> MOVZX_RR = {0x0F, 0xB7};
    static constexpr std::array<std::uint8_t, 2> MOVZX_RR_8 = {0x0F, 0xB6};

    template<typename SRC>
    class Movzx {
    public:
        template<typename S = SRC>
        explicit constexpr Movzx(const std::uint8_t from_size, const std::uint8_t to_size, S&& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_dest(dest),
            m_src(std::forward<S>(src)) {}

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            if (m_from_size != 1 && m_from_size != 2) {
                die("Invalid size for instruction: {}", m_from_size);
            }

            Encoder enc(buffer, MOVZX_RR_8, MOVZX_RR);
            return enc.encode_MR(m_to_size, m_from_size, m_dest, m_src);
        }

    protected:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        GPReg m_dest;
        SRC m_src;
    };

    class MovzxRR final: public Movzx<GPReg> {
    public:
        explicit constexpr MovzxRR(const std::uint8_t from_size, const std::uint8_t to_size, const GPReg& src, const GPReg& dest) noexcept:
            Movzx(from_size, to_size, src, dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovzxRR& movzxrr) {
            return os << "movz" << prefix_size(movzxrr.m_from_size)
                << prefix_size(movzxrr.m_to_size)
                << " %" << movzxrr.m_src.name(movzxrr.m_from_size)
                << ", %" << movzxrr.m_dest.name(movzxrr.m_to_size);
        }
    };

    class MovzxRM final: public Movzx<Address> {
    public:
        explicit constexpr MovzxRM(const std::uint8_t from_size, const std::uint8_t to_size, const Address& src, const GPReg& dest) noexcept:
            Movzx(from_size, to_size, src, dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovzxRM& movzxrm) {
            return os << "movz" << prefix_size(movzxrm.m_from_size)
                << prefix_size(movzxrm.m_to_size)
                << " " << movzxrm.m_src
                << ", %" << movzxrm.m_dest.name(movzxrm.m_to_size);
        }
    };
}