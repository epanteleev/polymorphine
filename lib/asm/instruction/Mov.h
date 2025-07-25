#pragma once
#include "asm/Register.h"

namespace aasm {
    class MovRR final {
    public:
        explicit constexpr MovRR(std::uint8_t size, const GPReg& src, const GPReg& dest) noexcept:
            m_size(size), m_src(src), m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovRR& movrr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            switch (m_size) {
                case 8: {
                    emit_mov(buffer, constants::REX_W);
                    break;
                }
                case 4: {
                    emit_mov(buffer, 0);
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    emit_mov(buffer, 0);
                    break;
                }
                case 1: {
                    emit_mov_byte(buffer);
                    break;
                }
                default: die("Invalid size for mov instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t MOV_RR = 0x89;
        static constexpr std::uint8_t MOV_RR_8 = 0x88;

        template<CodeBuffer Buffer>
        constexpr void emit_mov(Buffer& buffer, const std::uint8_t rex_w) const {
            buffer.emit8(constants::REX | B(m_dest) | rex_w | R(m_src));
            buffer.emit8(MOV_RR);
            buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dest));
        }

        template<CodeBuffer Buffer>
        constexpr void emit_mov_byte(Buffer& buffer) const {
            buffer.emit8(constants::REX | B(m_dest) | R(m_src));
            buffer.emit8(MOV_RR_8);
            buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dest));
        }

        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovRR &movrr) {
        return os << "mov" << prefix_size(movrr.m_size) << " %" << movrr.m_src.name(movrr.m_size) << ", %" << movrr.m_dest.name(movrr.m_size);
    }

    class MovRI final {
    public:
        constexpr MovRI(std::uint8_t size, const std::int64_t src, const GPReg& dest) noexcept:
            m_size(size), m_src(src), m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovRI &movri);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t MOV_RI_8 = 0xB0;
            static constexpr std::uint8_t MOV_RI = 0xB8;
            switch (m_size) {
                case 8: {
                    buffer.emit8(constants::REX | B(m_dest) | constants::REX_W);
                    buffer.emit8(MOV_RI | reg3(m_dest));
                    buffer.emit64(m_src);
                    break;
                }
                case 4: {
                    if (const auto rex = constants::REX | B(m_dest); rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(MOV_RI | reg3(m_dest));
                    buffer.emit32(checked_cast<std::int32_t>(m_src));
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    if (const auto rex = constants::REX | B(m_dest); rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(MOV_RI | reg3(m_dest));
                    buffer.emit16(checked_cast<std::int16_t>(m_src));
                    break;
                }
                case 1: {
                    if (const auto rex = constants::REX | B(m_dest); rex != constants::REX) {
                        buffer.emit8(rex);
                    }
                    buffer.emit8(MOV_RI_8 | reg3(m_dest));
                    buffer.emit8(checked_cast<std::int8_t>(m_src));
                    break;
                }
                default: die("Invalid size for mov instruction: {}", m_size);
            }
        }
    private:
        std::uint8_t m_size;
        std::int64_t m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovRI &movri) {
        if (movri.m_size == 8) {
            os << "movabs";
        } else {
            os << "mov";
        }

        return os << prefix_size(movri.m_size) << " $" << movri.m_src << ", %" << movri.m_dest.name(movri.m_size);
    }

    class MovMR final {
    public:
        constexpr MovMR(std::uint8_t size, const GPReg src, const Address& dst) noexcept:
            m_size(size), m_src(src), m_dest(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const MovMR &movmr);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t MOV_MR = 0x89;
            static constexpr std::uint8_t MOV_MR_8 = 0x88;
            switch (m_size) {
                case 1: {
                    const auto reg = constants::REX | R(m_src) | X(m_dest) | B(m_dest.base);
                    if (reg != constants::REX) {
                        buffer.emit8(reg);
                    }

                    buffer.emit8(MOV_MR_8);
                    m_dest.encode(buffer, reg3(m_src));
                    break;
                }
                case 2: add_word_op_size(buffer); [[fallthrough]];
                case 4: {
                    const auto reg = constants::REX | R(m_src) | X(m_dest) | B(m_dest.base);
                    if (reg != constants::REX) {
                        buffer.emit8(reg);
                    }

                    buffer.emit8(MOV_MR);
                    m_dest.encode(buffer, reg3(m_src));
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | R(m_src) | X(m_dest) | B(m_dest.base));
                    buffer.emit8(MOV_MR);
                    m_dest.encode(buffer, reg3(m_src));
                    break;
                }
                default: die("Invalid size for mov instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        const GPReg m_src;
        const Address m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovMR &movmr) {
        return os << "mov" << prefix_size(movmr.m_size) << " %" << movmr.m_src.name(8) << ", " << movmr.m_dest;
    }

    class MovRM final {
    public:
        constexpr MovRM(const std::uint8_t size, const Address& src, const GPReg dst) noexcept:
            m_src(src),
            m_size(size),
            m_dest(dst) {}

        friend std::ostream& operator<<(std::ostream &os, const MovRM &movrm);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t MOV_RM = 0x8B;
            static constexpr std::uint8_t MOV_RM_8 = 0x8A;
            switch (m_size) {
                case 1: {
                    const auto reg = constants::REX | R(m_dest) | X(m_src) | B(m_src.base);
                    if (reg != constants::REX || is_special_byte_reg(m_dest)) {
                        buffer.emit8(reg);
                    }

                    buffer.emit8(MOV_RM_8);
                    m_src.encode(buffer, reg3(m_dest));
                    break;
                }
                case 2: add_word_op_size(buffer); [[fallthrough]];
                case 4: {
                    const auto reg = constants::REX | R(m_dest) | X(m_src) | B(m_src.base);
                    if (reg != constants::REX) {
                        buffer.emit8(reg);
                    }

                    buffer.emit8(MOV_RM);
                    m_src.encode(buffer, reg3(m_dest));
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | R(m_dest) | X(m_src) | B(m_src.base));
                    buffer.emit8(MOV_RM);
                    m_src.encode(buffer, reg3(m_dest));
                    break;
                }
                default: die("Invalid size for mov instruction: {}", m_size);
            }
        }

    private:
        const Address m_src;
        std::uint8_t m_size;
        const GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovRM &movrm) {
        return os << "mov" << prefix_size(movrm.m_size) << " " << movrm.m_src << ", %" << movrm.m_dest.name(movrm.m_size);
    }

    class MovMI final {
    public:
        constexpr MovMI(const std::uint8_t size, const std::int32_t src, const Address& dst) noexcept:
            m_src(src),
            m_dest(dst),
            m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const MovMI &movrm);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t MOV_MI = 0xC7;
            static constexpr std::uint8_t MOV_MI_8 = 0xC6;
            switch (m_size) {
                case 1: {
                    if (const auto reg = constants::REX | X(m_dest) | B(m_dest.base); reg != constants::REX) {
                        buffer.emit8(reg);
                    }

                    buffer.emit8(MOV_MI_8);
                    m_dest.encode(buffer, 0);
                    buffer.emit8(checked_cast<std::int8_t>(m_src));
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    if (const auto reg = constants::REX | X(m_dest) | B(m_dest.base); reg != constants::REX) {
                        buffer.emit8(reg);
                    }

                    buffer.emit8(MOV_MI);
                    m_dest.encode(buffer, 0);
                    buffer.emit16(checked_cast<std::int16_t>(m_src));
                    break;
                }
                case 4: {
                    if (const auto reg = constants::REX | X(m_dest) | B(m_dest.base); reg != constants::REX) {
                        buffer.emit8(reg);
                    }

                    buffer.emit8(MOV_MI);
                    m_dest.encode(buffer, 0);
                    buffer.emit32(checked_cast<std::int32_t>(m_src));
                    break;
                }
                case 8: {
                    buffer.emit8(constants::REX_W | X(m_dest) | B(m_dest.base));
                    buffer.emit8(MOV_MI);
                    m_dest.encode(buffer, 0);
                    buffer.emit32(m_src);
                    break;
                }
                default: die("Invalid size for mov instruction: {}", m_size);
            }
        }

    private:
        const std::int32_t m_src;
        const Address m_dest;
        std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovMI &movrm) {
        return os << "mov" << prefix_size(movrm.m_size) << " $" << movrm.m_src << ", " << movrm.m_dest;
    }
}
