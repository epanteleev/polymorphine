#pragma once

#include "Common.h"
#include "address/Address.h"
#include "Register.h"

namespace aasm {
    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_MR(Buffer& buffer, const std::uint8_t size, const GPReg src, const Address& dest) {
        switch (size) {
            case 1: {
                const auto reg = constants::REX | R(src) | X(dest) | B(dest.base());
                if (reg != constants::REX || is_special_byte_reg(src)) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                dest.encode(buffer, reg3(src));
                break;
            }
            case 2: add_word_op_size(buffer); [[fallthrough]];
            case 4: {
                const auto reg = constants::REX | R(src) | X(dest) | B(dest.base());
                if (reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(CODING);
                dest.encode(buffer, reg3(src));
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | R(src) | X(dest) | B(dest.base()));
                buffer.emit8(CODING);
                dest.encode(buffer, reg3(src));
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_RM(Buffer& buffer, const std::uint8_t size, const Address& src, const GPReg dest) {
        switch (size) {
            case 1: {
                const auto reg = constants::REX | R(dest) | X(src) | B(src.base());
                if (reg != constants::REX || is_special_byte_reg(dest)) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                src.encode(buffer, reg3(dest));
                break;
            }
            case 2: add_word_op_size(buffer); [[fallthrough]];
            case 4: {
                const auto reg = constants::REX | R(dest) | X(src) | B(src.base());
                if (reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(CODING);
                src.encode(buffer, reg3(dest));
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | R(dest) | X(src) | B(src.base()));
                buffer.emit8(CODING);
                src.encode(buffer, reg3(dest));
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_RR(Buffer& buffer, const std::uint8_t size, const GPReg src, const GPReg dest) {
        switch (size) {
            case 1: {
                const auto reg = constants::REX | B(dest) | R(src);
                if (reg != constants::REX || is_special_byte_reg(dest) || is_special_byte_reg(src)) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                buffer.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
                break;
            }
            case 2: add_word_op_size(buffer); [[fallthrough]];
            case 4: {
                const auto reg = constants::REX | B(dest) | R(src);
                if (reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(CODING);
                buffer.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | B(dest) | R(src));
                buffer.emit8(CODING);
                buffer.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_RI32_cmp(Buffer& buffer, const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
        switch (size) {
            case 1: {
                const auto reg = constants::REX | B(dst);
                if (reg != constants::REX || is_special_byte_reg(dst)) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                buffer.emit8(0xC0 | reg3(dst) | 0x38);
                buffer.emit8(checked_cast<std::int8_t>(imm));
                break;
            }
            case 2: {
                add_word_op_size(buffer);
                if (const auto rex = constants::REX | B(dst); rex != constants::REX) {
                    buffer.emit8(rex);
                }

                if (std::in_range<std::int8_t>(imm)) {
                    buffer.emit8(CODING | 0x02);
                    buffer.emit8(0xC0 | reg3(dst) | 0x38);
                    buffer.emit8(static_cast<std::int8_t>(imm));

                } else {
                    buffer.emit8(CODING);
                    buffer.emit8(0xC0 | reg3(dst) | 0x38);
                    buffer.emit16(checked_cast<std::int16_t>(imm));
                }
                break;
            }
            case 4: {
                if (const auto reg = constants::REX | B(dst); reg != constants::REX) {
                    buffer.emit8(reg);
                }

                if (std::in_range<std::int8_t>(imm)) {
                    buffer.emit8(CODING | 0x02);
                    buffer.emit8(0xC0 | reg3(dst) | 0x38);
                    buffer.emit8(static_cast<std::int8_t>(imm));
                } else {
                    buffer.emit8(CODING);
                    buffer.emit8(0xC0 | reg3(dst) | 0x38);
                    buffer.emit32(checked_cast<std::int32_t>(imm));
                }
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | B(dst));
                if (std::in_range<std::int8_t>(imm)) {
                    buffer.emit8(CODING | 0x02);
                    buffer.emit8(0xC0 | reg3(dst) | 0x38);
                    buffer.emit8(static_cast<std::int8_t>(imm));
                } else {
                    buffer.emit8(CODING);
                    buffer.emit8(0xC0 | reg3(dst) | 0x38);
                    buffer.emit32(checked_cast<std::int32_t>(imm));
                }
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, std::uint8_t MODRM, CodeBuffer Buffer>
    constexpr void encode_RI32_arithmetic(Buffer &buffer, const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
        switch (size) {
            case 1: {
                const auto rex = constants::REX | B(dst);
                if (rex != constants::REX || is_special_byte_reg(dst)) {
                    buffer.emit8(rex);
                }
                buffer.emit8(B_CODING);
                buffer.emit8(0xC0 | reg3(dst) | MODRM);
                buffer.emit8(static_cast<std::int8_t>(imm));
                break;
            }
            case 2: {
                add_word_op_size(buffer);
                const auto rex = constants::REX | B(dst);
                if (rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(CODING);
                buffer.emit8(0xC0 | reg3(dst) | MODRM);
                buffer.emit16(static_cast<std::int16_t>(imm));
                break;
            }
            case 4: {
                const auto rex = constants::REX | B(dst);
                if (rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(CODING);
                buffer.emit8(0xC0 | reg3(dst) | MODRM);
                buffer.emit32(static_cast<std::int32_t>(imm));
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | B(dst) | constants::REX);
                buffer.emit8(CODING);
                buffer.emit8(0xC0 | reg3(dst) | MODRM);
                buffer.emit32(static_cast<std::int32_t>(imm));
                break;
            }
            default: die("Invalid size for add instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_MI32_cmp(Buffer& buffer, const std::uint8_t size, const std::int32_t imm, const Address& dst) {
        switch (size) {
            case 1: {
                const auto rex = constants::REX | X(dst) | B(dst.base());
                if (rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(B_CODING);
                dst.encode(buffer,  7);
                buffer.emit8(checked_cast<std::int8_t>(imm));
                break;
            }
            case 2: {
                add_word_op_size(buffer);
                if (const auto rex = constants::REX | X(dst) | B(dst.base()); rex != constants::REX) {
                    buffer.emit8(rex);
                }
                if (std::in_range<std::int8_t>(imm)) {
                    buffer.emit8(CODING | 0x02);
                    dst.encode(buffer, 7);
                    buffer.emit8(static_cast<std::int8_t>(imm));

                } else {
                    buffer.emit8(CODING);
                    dst.encode(buffer, 7);
                    buffer.emit16(checked_cast<std::int16_t>(imm));
                }

                break;
            }
            case 4: {
                if (const auto rex = constants::REX | X(dst) | B(dst.base()); rex != constants::REX) {
                    buffer.emit8(rex);
                }

                if (std::in_range<std::int8_t>(imm)) {
                    buffer.emit8(CODING | 0x02);
                    dst.encode(buffer, 7);
                    buffer.emit8(static_cast<std::int8_t>(imm));
                } else {
                    buffer.emit8(CODING);
                    dst.encode(buffer, 7);
                    buffer.emit32(checked_cast<std::int32_t>(imm));
                }
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | X(dst) | B(dst.base()));
                if (std::in_range<std::int8_t>(imm)) {
                    buffer.emit8(CODING | 0x02);
                    dst.encode(buffer, 7);
                    buffer.emit8(static_cast<std::int8_t>(imm));
                } else {
                    buffer.emit8(CODING);
                    dst.encode(buffer, 7);
                    buffer.emit32(checked_cast<std::int32_t>(imm));
                }
                break;
            }
            default: die("Invalid size for cmp instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_RI64(Buffer& buffer, const std::uint8_t size, const std::int64_t imm, const GPReg dst) {
        switch (size) {
            case 8: {
                buffer.emit8(constants::REX | B(dst) | constants::REX_W);
                buffer.emit8(CODING | reg3(dst));
                buffer.emit64(imm);
                break;
            }
            case 4: {
                if (const auto rex = constants::REX | B(dst); rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(CODING | reg3(dst));
                buffer.emit32(checked_cast<std::int32_t>(imm));
                break;
            }
            case 2: {
                add_word_op_size(buffer);
                if (const auto rex = constants::REX | B(dst); rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(CODING | reg3(dst));
                buffer.emit16(checked_cast<std::int16_t>(imm));
                break;
            }
            case 1: {
                if (const auto rex = constants::REX | B(dst); rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(B_CODING | reg3(dst));
                buffer.emit8(checked_cast<std::int8_t>(imm));
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_RI32(Buffer& buffer, const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
        switch (size) {
            case 8: {
                buffer.emit8(constants::REX | B(dst) | constants::REX_W);
                buffer.emit8(CODING | reg3(dst));
                buffer.emit32(checked_cast<std::int32_t>(imm));
                break;
            }
            case 4: {
                if (const auto rex = constants::REX | B(dst); rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(CODING | reg3(dst));
                buffer.emit32(checked_cast<std::int32_t>(imm));
                break;
            }
            case 2: {
                add_word_op_size(buffer);
                if (const auto rex = constants::REX | B(dst); rex != constants::REX) {
                    buffer.emit8(rex);
                }
                buffer.emit8(CODING | reg3(dst));
                buffer.emit16(checked_cast<std::int16_t>(imm));
                break;
            }
            case 1: {
                const auto rex = constants::REX | B(dst);
                if (rex != constants::REX || is_special_byte_reg(dst)) {
                    buffer.emit8(rex);
                }
                buffer.emit8(B_CODING);
                buffer.emit8(0xC0 | reg3(dst));
                buffer.emit8(checked_cast<std::int8_t>(imm));
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, std::uint8_t MODRM, CodeBuffer Buffer>
    constexpr void encode_MI32(Buffer& buffer, const std::uint8_t size, const std::int32_t imm, const Address& dst) {
        switch (size) {
            case 1: {
                const auto reg = constants::REX | X(dst) | B(dst.base());
                if (reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                dst.encode(buffer, MODRM >> 3);
                buffer.emit8(checked_cast<std::int8_t>(imm));
                break;
            }
            case 2: {
                add_word_op_size(buffer);
                if (const auto reg = constants::REX | X(dst) | B(dst.base()); reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(CODING);
                dst.encode(buffer, MODRM >> 3);
                buffer.emit16(checked_cast<std::int16_t>(imm));
                break;
            }
            case 4: {
                if (const auto reg = constants::REX | X(dst) | B(dst.base()); reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(CODING);
                dst.encode(buffer, MODRM >> 3);
                buffer.emit32(checked_cast<std::int32_t>(imm));
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | X(dst) | B(dst.base()));
                buffer.emit8(CODING);
                dst.encode(buffer, MODRM >> 3);
                buffer.emit32(imm);
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }
}