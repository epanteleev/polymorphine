#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> NEG_R_8 = {0xF6};
    static constexpr std::array<std::uint8_t, 1> NEG_R = {0xF7};

    template<typename SRC>
    class Neg {
    public:
        template<typename S = SRC>
        explicit constexpr Neg(const std::uint8_t size, S&& reg) noexcept:
            m_size(size),
            m_src(std::forward<S>(reg)) {}

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer &buffer) const {
            Encoder enc(buffer, NEG_R_8, NEG_R);
            switch (m_size) {
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 4: [[fallthrough]];
                case 8: return enc.encode_M(3, m_size, m_src);
                default: die("Invalid size for neg instruction: {}", m_size);
            }
        }

    protected:
        std::uint8_t m_size;
        SRC m_src;
    };

    class NegR final: public Neg<GPReg> {
    public:
        explicit constexpr NegR(const std::uint8_t size, const GPReg reg) noexcept:
            Neg(size, reg) {}

        friend std::ostream& operator<<(std::ostream& os, const NegR& negr);
    };

    class NegM final: public Neg<Address> {
    public:
        explicit constexpr NegM(const std::uint8_t size, const Address& addr) noexcept:
            Neg(size, addr) {}

        friend std::ostream& operator<<(std::ostream& os, const NegM &negm);
    };
}