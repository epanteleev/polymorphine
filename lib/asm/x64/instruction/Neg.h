#pragma once

namespace aasm::details {
    static constexpr std::array<std::uint8_t, 1> NEG_R_8 = {0xF6};
    static constexpr std::array<std::uint8_t, 1> NEG_R = {0xF7};

    class NegR final {
    public:
        explicit constexpr NegR(const std::uint8_t size, const GPReg reg) noexcept:
            m_size(size),
            m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream& os, const NegR& negr);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer &buffer) const {
            Encoder enc(buffer, NEG_R_8, NEG_R);
            switch (m_size) {
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 4: [[fallthrough]];
                case 8: return enc.encode_M(3, m_size, m_reg);
                default: die("Invalid size for neg instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        GPReg m_reg;
    };

    inline std::ostream & operator<<(std::ostream &os, const NegR &negr) {
        return os << "neg" << prefix_size(negr.m_size) << " %" << negr.m_reg.name(negr.m_size);
    }

    class NegM final {
    public:
        explicit constexpr NegM(const std::uint8_t size, const Address& addr) noexcept:
            m_size(size),
            m_addr(addr) {}

        friend std::ostream& operator<<(std::ostream& os, const NegM &negm);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer &buffer) const {
            static constexpr std::array<std::uint8_t, 1> NEG_M_8 = {0xF6};
            static constexpr std::array<std::uint8_t, 1> NEG_M = {0xF7};
            Encoder enc(buffer, NEG_M_8, NEG_M);
            switch (m_size) {
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 4: [[fallthrough]];
                case 8: return enc.encode_M(3, m_size, m_addr);
                default: die("Invalid size for neg instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        Address m_addr;
    };

    inline std::ostream & operator<<(std::ostream &os, const NegM &negm) {
        return os << "neg" << prefix_size(negm.m_size) << ' ' << negm.m_addr;
    }
}
