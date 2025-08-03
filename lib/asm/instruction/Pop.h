#pragma once

namespace aasm::details {
    class PopR final {
    public:
        constexpr PopR(const std::uint8_t size, const GPReg reg) noexcept
            : m_size(size), m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream& os, const PopR& popr);

        template<CodeBuffer C>
        constexpr void emit(C &c) const {
            static constexpr std::uint8_t POP_R = 0x58;
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: details::encode_O<POP_R>(c, m_size, m_reg); break;
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        GPReg m_reg;
    };

    inline std::ostream & operator<<(std::ostream &os, const PopR &popr) {
        return os << "pop" << prefix_size(popr.m_size) << ' ' << popr.m_reg.name(popr.m_size);
    }

    class PopM final {
    public:
        constexpr PopM(std::uint8_t size, const Address& addr) noexcept:
            m_size(size),
            m_addr(addr) {}

        friend std::ostream& operator<<(std::ostream& os, const PopM& popm);

        template<CodeBuffer C>
        constexpr void emit(C &c) const {
            static constexpr std::uint8_t POP_M = 0x8F;
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: {
                    details::encode_M<POP_M, POP_M, 0>(c, m_size, m_addr);
                    break;
                }
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