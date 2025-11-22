#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> DIV_8 = {0xf6};
    static constexpr std::array<std::uint8_t, 1> DIV = {0xf7};

    template<std::uint8_t MODRM, typename SRC>
    class Div {
    public:
        template<typename S = SRC>
        constexpr explicit Div(const std::uint8_t size, S&& divisor) noexcept:
            m_size(size),
            m_divisor(std::forward<S>(divisor)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            Encoder enc(buffer, DIV_8, DIV);
            return enc.encode_M(MODRM, m_size, m_divisor);
        }

    protected:
        std::uint8_t m_size;
        SRC m_divisor;
    };

    class UDivR final: public Div<6, GPReg> {
    public:
        constexpr explicit UDivR(const std::uint8_t size, const GPReg divisor) noexcept:
            Div(size, divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const UDivR& idiv);
    };

    class UDivM final: public Div<6, Address> {
    public:
        constexpr explicit UDivM(const std::uint8_t size, const Address& divisor) noexcept:
            Div(size, divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const UDivM &idiv);
    };

    class IdivR final: public Div<7, GPReg> {
    public:
        constexpr explicit IdivR(const std::uint8_t size, const GPReg divisor) noexcept:
            Div(size, divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const IdivR& idiv);
    };

    class IdivM final: public Div<7, Address> {
    public:
        constexpr explicit IdivM(const std::uint8_t size, const Address& divisor) noexcept:
            Div(size, divisor) {}

        friend std::ostream& operator<<(std::ostream &os, const IdivM &idiv);
    };
}