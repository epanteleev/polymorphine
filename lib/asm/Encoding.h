#pragma once

#include "Common.h"
#include "Address.h"
#include "Register.h"

namespace aasm {
    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_MR(Buffer& buffer, const std::uint8_t size, const GPReg src, const Address& dest) {
        switch (size) {
            case 1: {
                const auto reg = constants::REX | R(src) | X(dest) | B(dest.base);
                if (reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                dest.encode(buffer, reg3(src));
                break;
            }
            case 2: add_word_op_size(buffer); [[fallthrough]];
            case 4: {
                const auto reg = constants::REX | R(src) | X(dest) | B(dest.base);
                if (reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(CODING);
                dest.encode(buffer, reg3(src));
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | R(src) | X(dest) | B(dest.base));
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
                const auto reg = constants::REX | R(dest) | X(src) | B(src.base);
                if (reg != constants::REX || is_special_byte_reg(dest)) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                src.encode(buffer, reg3(dest));
                break;
            }
            case 2: add_word_op_size(buffer); [[fallthrough]];
            case 4: {
                const auto reg = constants::REX | R(dest) | X(src) | B(src.base);
                if (reg != constants::REX) {
                    buffer.emit8(reg);
                }

                buffer.emit8(CODING);
                src.encode(buffer, reg3(dest));
                break;
            }
            case 8: {
                buffer.emit8(constants::REX_W | R(dest) | X(src) | B(src.base));
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
                const auto reg = constants::REX | R(dest) | R(src);
                if (reg != constants::REX || is_special_byte_reg(dest) || is_special_byte_reg(src)) {
                    buffer.emit8(reg);
                }

                buffer.emit8(B_CODING);
                buffer.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
                break;
            }
            case 2: add_word_op_size(buffer); [[fallthrough]];
            case 4: {
                const auto reg = constants::REX | R(dest) | R(src);
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
}