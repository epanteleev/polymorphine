#pragma once

namespace aasm::details {
    class PopR final {
    public:
        constexpr PopR(const std::uint8_t size, const GPReg reg) noexcept:
            m_size(size),
            m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream& os, const PopR& popr);

        template<CodeBuffer C>
        constexpr void emit(C &c) const {
            static constexpr std::array<std::uint8_t, 1> POP_R = {0x58};
            Encoder enc(c, POP_R, POP_R);
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: enc.encode_O(m_size, m_reg); break;
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        GPReg m_reg;
    };

    inline std::ostream & operator<<(std::ostream &os, const PopR &popr) {
        return os << "pop" << prefix_size(popr.m_size) << " %" << popr.m_reg.name(popr.m_size);
    }

    class PopM final {
    public:
        constexpr PopM(const std::uint8_t size, const Address& addr) noexcept:
            m_size(size),
            m_addr(addr) {}

        friend std::ostream& operator<<(std::ostream& os, const PopM& popm);

        template<CodeBuffer C>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(C &c) const {
            static constexpr std::array<std::uint8_t, 1> POP_M = {0x8F};
            Encoder enc(c, POP_M, POP_M);
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: return enc.encode_M_without_REXW(0, m_size, m_addr);
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        Address m_addr;
    };

    inline std::ostream & operator<<(std::ostream &os, const PopM &popm) {
        return os << "pop" << prefix_size(popm.m_size) << ' ' << popm.m_addr;
    }
}