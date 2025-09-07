#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> IDIV_8 = {0xf6};
    static constexpr std::array<std::uint8_t, 1> IDIV = {0xf7};

    class IdivR final {
    public:
        constexpr explicit IdivR(const std::uint8_t size, const GPReg divisor) noexcept:
            m_size(size),
            m_divisor(divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const IdivR& idiv);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            Encoder enc(buffer, IDIV_8, IDIV);
            enc.encode_M(7, m_size, m_divisor);
        }

    private:
        std::uint8_t m_size;
        GPReg m_divisor;
    };

    inline std::ostream & operator<<(std::ostream &os, const IdivR &idiv) {
        return os << "idiv" << prefix_size(idiv.m_size) << " %"
                   << idiv.m_divisor.name(idiv.m_size);
    }

    class IdivM final {
    public:
        constexpr explicit IdivM(const std::uint8_t size, const Address& divisor) noexcept:
            m_size(size),
            m_divisor(divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const IdivM &idiv);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            Encoder enc(buffer, IDIV_8, IDIV);
            return enc.encode_M_with_REXW(7, m_size, m_divisor);
        }

    private:
        std::uint8_t m_size;
        Address m_divisor;
    };

    inline std::ostream & operator<<(std::ostream &os, const IdivM &idiv) {
        return os << "idiv" << prefix_size(idiv.m_size) << ' ' << idiv.m_divisor;
    }
}