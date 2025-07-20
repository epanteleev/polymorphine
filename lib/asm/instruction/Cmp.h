#pragma once

namespace aasm {
    class CmpRR final {
    public:
        constexpr CmpRR(const std::uint8_t size, const GPReg src, const GPReg dst) noexcept:
            m_size(size), m_src(src), m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRR& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMP_RR = 0x39;
            static constexpr std::uint8_t CMP_RR_8 = 0x38;

            switch (m_size) {
                case 1: {
                    const auto rex = constants::REX | B(m_dst) | R(m_src);
                    if (rex != constants::REX || is_special_byte_reg(m_dst) || is_special_byte_reg(m_src)) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(CMP_RR_8);
                    buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dst));
                    break;
                }
                case 2: add_word_op_size(buffer); [[fallthrough]];
                case 4: {
                    const auto rex = constants::REX | B(m_dst) | R(m_src);
                    if (rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(CMP_RR);
                    buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dst));
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | B(m_dst) | R(m_src));
                    buffer.emit8(CMP_RR);
                    buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dst));
                    break;
                }
                default: die("Invalid size for cmp instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpRR &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " %"
                  << cmp.m_src.name(cmp.m_size) << ", %"
                  << cmp.m_dst.name(cmp.m_size);
    }

    class CmpRI final {
    public:
        constexpr CmpRI(const std::uint8_t size, const std::int32_t imm, const GPReg dst) noexcept:
            m_imm(imm),
            m_size(size),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRI& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            switch (m_size) {
                case 1: {
                    const auto rex = constants::REX | B(m_dst);
                    if (rex != constants::REX || is_special_byte_reg(m_dst)) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(CMP_RI_8);
                    buffer.emit8(0xC0 | reg3(m_dst) | 0x38);
                    buffer.emit8(checked_cast<std::int8_t>(m_imm));
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    if (const auto rex = constants::REX | B(m_dst); rex != constants::REX) {
                        buffer.emit8(rex);
                    }

                    if (std::in_range<std::int8_t>(m_imm)) {
                        buffer.emit8(CMP_RI | 0x02);
                        buffer.emit8(0xC0 | reg3(m_dst) | 0x38);
                        buffer.emit8(static_cast<std::int8_t>(m_imm));

                    } else {
                        buffer.emit8(CMP_RI);
                        buffer.emit8(0xC0 | reg3(m_dst) | 0x38);
                        buffer.emit16(checked_cast<std::int16_t>(m_imm));
                    }
                    break;
                }
                case 4: {
                    if (const auto rex = constants::REX | B(m_dst); rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    emit_cmp_32_and_64(buffer);
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | B(m_dst));
                    emit_cmp_32_and_64(buffer);
                    break;
                }
                default: die("Invalid size for cmp instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t CMP_RI = 0x81;
        static constexpr std::uint8_t CMP_RI_8 = 0x80;

        template<CodeBuffer Buffer>
        constexpr void emit_cmp_32_and_64(Buffer& buffer) const {
            if (std::in_range<std::int8_t>(m_imm)) {
                buffer.emit8(CMP_RI | 0x02);
                buffer.emit8(0xC0 | reg3(m_dst) | 0x38);
                buffer.emit8(static_cast<std::int8_t>(m_imm));
            } else {
                buffer.emit8(CMP_RI);
                buffer.emit8(0xC0 | reg3(m_dst) | 0x38);
                buffer.emit32(checked_cast<std::int32_t>(m_imm));
            }
        }

        std::int32_t m_imm;
        std::uint8_t m_size;
        GPReg m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpRI &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " $"
                  << cmp.m_imm << ", %" << cmp.m_dst.name(cmp.m_size);
    }
}
