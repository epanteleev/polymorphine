#pragma once

namespace aasm {
    class PopR final {
    public:
        constexpr PopR(const std::uint8_t size, const GPReg reg) noexcept
            : m_size(size), m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream& os, const PopR& popr);

        template<CodeBuffer C>
        constexpr void emit(C &c) const {
            switch (m_size) {
                case 8: emit_pop(c); break;
                case 2: {
                    add_word_op_size(c);
                    emit_pop(c);
                    break;
                }
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t POP_R = 0x58;

        template<CodeBuffer C>
        constexpr void emit_pop(C& c) const {
            if (const auto rex = constants::REX | B(m_reg); rex != constants::REX) {
                c.emit8(rex);
            }

            c.emit8(POP_R + reg3(m_reg));
        }

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
            switch (m_size) {
                case 8: emit_pop(c); break;
                case 2: {
                    add_word_op_size(c);
                    emit_pop(c);
                    break;
                }
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t POP_M = 0x8F;

        template<CodeBuffer C>
        constexpr void emit_pop(C& c) const noexcept {
            unsigned char rex = constants::REX | X(m_addr) | B(m_addr.base);
            if (rex != constants::REX) {
                c.emit8(rex);
            }

            c.emit8(POP_M);
            m_addr.encode(c, 0);
        }

        std::uint8_t m_size;
        Address m_addr;
    };

    inline std::ostream & operator<<(std::ostream &os, const PopM &popm) {
        return os << "pop" << prefix_size(popm.m_size) << ' ' << popm.m_addr;
    }
}
