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
            encode_RR<CMP_RR_8, CMP_RR>(buffer, m_size, m_src, m_dst);
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
                        buffer.emit8(CMP_RI | BYTE_MARK);
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
        static constexpr std::uint8_t BYTE_MARK = 0x02;

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

    class CmpRM final {
    public:
        constexpr CmpRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_size(size),
            m_dst(dst),
            m_src(src) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpRM& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMP_MR = 0x3B;
            static constexpr std::uint8_t CMP_MR_8 = 0x3A;
            encode_RM<CMP_MR_8, CMP_MR>(buffer, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_dst;
        Address m_src;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpRM &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << ' '
                  << cmp.m_src << ", %" << cmp.m_dst.name(cmp.m_size);
    }

    class CmpMR final {
    public:
        constexpr CmpMR(const std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            m_size(size),
            m_src(src),
            m_dst(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpMR& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CMP_MR = 0x3B;
            static constexpr std::uint8_t CMP_MR_8 = 0x3A;
            encode_MR<CMP_MR_8, CMP_MR>(buffer, m_size, m_src, m_dst);
        }

    private:
        std::uint8_t m_size;
        GPReg m_src;
        Address m_dst;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpMR &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " %" << cmp.m_src.name(cmp.m_size) << ", " << cmp.m_dst;
    }

    class CmpMI final {
    public:
        constexpr CmpMI(const std::uint8_t size, const std::int32_t imm, const Address& second) noexcept:
            m_second(second),
            m_imm(imm),
            m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const CmpMI& cmp);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            switch (m_size) {
                case 1: {
                    const auto rex = constants::REX | X(m_second) | B(m_second.base);
                    if (rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(CMP_MI_8);
                    m_second.encode(buffer,  7);
                    buffer.emit8(checked_cast<std::int8_t>(m_imm));
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    if (const auto rex = constants::REX | X(m_second) | B(m_second.base); rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    if (std::in_range<std::int8_t>(m_imm)) {
                        buffer.emit8(CMP_MI | 0x02);
                        m_second.encode(buffer, 7);
                        buffer.emit8(static_cast<std::int8_t>(m_imm));

                    } else {
                        buffer.emit8(CMP_MI);
                        m_second.encode(buffer, 7);
                        buffer.emit16(checked_cast<std::int16_t>(m_imm));
                    }

                    break;
                }
                case 4: {
                    if (const auto rex = constants::REX | X(m_second) | B(m_second.base); rex != constants::REX) {
                        buffer.emit8(rex);
                    }

                    emit_cmp_32_and_64(buffer);
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | X(m_second) | B(m_second.base));
                    emit_cmp_32_and_64(buffer);
                    break;
                }
                default: die("Invalid size for cmp instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t CMP_MI = 0x81;
        static constexpr std::uint8_t CMP_MI_8 = 0x80;

        template<CodeBuffer Buffer>
        constexpr void emit_cmp_32_and_64(Buffer& buffer) const {
            if (std::in_range<std::int8_t>(m_imm)) {
                buffer.emit8(CMP_MI | 0x02);
                m_second.encode(buffer, 7);
                buffer.emit8(static_cast<std::int8_t>(m_imm));
            } else {
                buffer.emit8(CMP_MI);
                m_second.encode(buffer, 7);
                buffer.emit32(checked_cast<std::int32_t>(m_imm));
            }
        }

        Address m_second;
        std::int32_t m_imm;
        std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &os, const CmpMI &cmp) {
        return os << "cmp" << prefix_size(cmp.m_size)
            << " $" << cmp.m_imm << ", " << cmp.m_second;
    }
}
