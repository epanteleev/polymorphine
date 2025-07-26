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
            encode_RR<ADD_RR_8, ADD_RR>(buffer, m_size, m_src, m_dst);
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

    class AddRI final {
    public:
        explicit AddRI(const std::uint8_t size, const std::int32_t src, const GPReg dst) noexcept
            : m_size(size), m_src(src), m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddRI& add);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t ADD_RI = 0x81;
            static constexpr std::uint8_t ADD_RI_8 = 0x80;
            switch (m_size) {
                case 1: {
                    const auto rex = constants::REX | B(m_dst);
                    if (rex != constants::REX || is_special_byte_reg(m_dst)) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(ADD_RI_8);
                    buffer.emit8(0xC0 | reg3(m_dst));
                    buffer.emit8(static_cast<std::int8_t>(m_src));
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    const auto rex = constants::REX | B(m_dst);
                    if (rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(ADD_RI | reg3(m_dst));
                    buffer.emit16(static_cast<std::int16_t>(m_src));
                    break;
                }
                case 4: {
                    const auto rex = constants::REX | B(m_dst);
                    if (rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(ADD_RI);
                    buffer.emit8(0xC0 | reg3(m_dst));
                    buffer.emit32(static_cast<std::int32_t>(m_src));
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | B(m_dst) | constants::REX);
                    buffer.emit8(ADD_RI);
                    buffer.emit8(0xC0 | reg3(m_dst));
                    buffer.emit32(static_cast<std::int32_t>(m_src));
                    break;
                }
                default: die("Invalid size for add instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        std::int32_t m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddRI &add) {
        return os << "add" << prefix_size(add.m_size) << " $"
                  << add.m_src << ", %" << add.m_dst.name(add.m_size);
    }

    class AddRM final {
    public:
        explicit AddRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const AddRM& add);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t ADD_RM = 0x03;
            static constexpr std::uint8_t ADD_RM_8 = 0x02;
            encode_RM<ADD_RM_8, ADD_RM>(buffer, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        Address m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const AddRM &add) {
        return os << "add" << prefix_size(add.m_size) << " "
                  << add.m_src << ", %" << add.m_dst.name(add.m_size);
    }
}
