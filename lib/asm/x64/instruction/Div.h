#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> DIV_8 = {0xf6};
    static constexpr std::array<std::uint8_t, 1> DIV = {0xf7};

    class UDivR final {
    public:
        constexpr explicit UDivR(const std::uint8_t size, const GPReg divisor) noexcept:
            m_size(size),
            m_divisor(divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const UDivR& idiv);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            Encoder enc(buffer, DIV_8, DIV);
            enc.encode_M(6, m_size, m_divisor);
        }

    private:
        std::uint8_t m_size;
        GPReg m_divisor;
    };

    inline std::ostream & operator<<(std::ostream &os, const UDivR &idiv) {
        return os << "div" << prefix_size(idiv.m_size) << " %"
                   << idiv.m_divisor.name(idiv.m_size);
    }

    class UDivM final {
    public:
        constexpr explicit UDivM(const std::uint8_t size, const Address& divisor) noexcept:
            m_size(size),
            m_divisor(divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const UDivM &idiv);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            Encoder enc(buffer, DIV_8, DIV);
            return enc.encode_M_with_REXW(6, m_size, m_divisor);
        }

    private:
        std::uint8_t m_size;
        Address m_divisor;
    };

    inline std::ostream & operator<<(std::ostream &os, const UDivM &idiv) {
        return os << "div" << prefix_size(idiv.m_size) << ' ' << idiv.m_divisor;
    }
}