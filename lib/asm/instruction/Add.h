#pragma once


namespace aasm {
    class AddRR final {
    public:
        constexpr AddRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept
            : m_size(size), m_src(src), m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddRR& add);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t ADD_RR = 0x01;
            static constexpr std::uint8_t ADD_RR_8 = 0x00;
            switch (m_size) {
                case 1: {
                    const auto rex = constants::REX | B(m_dst) | R(m_src);
                    if (rex != constants::REX || is_special_byte_reg(m_dst) || is_special_byte_reg(m_src)) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(ADD_RR_8);
                    buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dst));
                    break;
                }
                case 2: add_word_op_size(buffer); [[fallthrough]];
                case 4: {
                    const auto rex = constants::REX | B(m_dst) | R(m_src);
                    if (rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(ADD_RR);
                    buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dst));
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | B(m_dst) | R(m_src));
                    buffer.emit8(ADD_RR);
                    buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dst));
                    break;
                }
                default: die("Invalid size for add instruction: {}", m_size);
            }
        }
    private:
        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddRR &add) {
        return os << "add" << prefix_size(add.m_size) << " %"
                  << add.m_src.name(add.m_size) << ", %"
                  << add.m_dst.name(add.m_size);
    }

}
