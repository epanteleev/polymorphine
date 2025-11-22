#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> MOVSXD = {0x63};

    template<typename SRC>
    class Movsxd {
    public:
        template<typename S = SRC>
        explicit constexpr Movsxd(const std::uint8_t from_size, const std::uint8_t to_size, S&& src, const GPReg& dest) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_dest(dest),
            m_src(std::forward<S>(src)) {}

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            if (m_from_size != 4 && m_to_size != 8) {
                die("Invalid size for instruction: {}", m_from_size);
            }

            Encoder enc(buffer, {}, MOVSXD);
            return enc.encode_MR(m_to_size, m_from_size, m_dest, m_src);
        }

    protected:
        std::uint8_t m_from_size;
        std::uint8_t m_to_size;
        GPReg m_dest;
        SRC m_src;
    };

    class MovsxdRR final: public Movsxd<GPReg> {
    public:
        explicit constexpr MovsxdRR(const std::uint8_t from_size, const std::uint8_t to_size, const GPReg& src, const GPReg& dest) noexcept:
            Movsxd(from_size, to_size, src, dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovsxdRR& movsxrr);
    };

    class MovsxdRM final: public Movsxd<Address> {
    public:
        explicit constexpr MovsxdRM(const std::uint8_t from_size, const std::uint8_t to_size, const Address& src, const GPReg& dest) noexcept:
            Movsxd(from_size, to_size, src, dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovsxdRM& movsxrm);
    };
}