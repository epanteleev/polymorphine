#pragma once

#include "Common.h"
#include "address/Address.h"
#include "Register.h"

namespace aasm::details {
    constexpr std::optional<std::uint8_t> prefix(std::uint8_t size, const GPReg src, const Address& dest) noexcept {
        auto code = R(src) | X(dest);
        if (const auto base = dest.base(); base.has_value()) {
            code |= B(base.value());
        }
        if (size == 8) {
            code |= constants::REX_W;
        } else {
            code |= constants::REX;
        }

        if (code != constants::REX || (size == 1 && is_special_byte_reg(src))) {
            return code;
        }

        return std::nullopt;
    }

    constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const GPReg src, const GPReg dest) noexcept {
        auto code = R(src) | B(dest);
        if (size == 8) {
            code |= constants::REX_W;
        } else {
            code |= constants::REX;
        }

        if (code != constants::REX || (size == 1 && (is_special_byte_reg(dest) || is_special_byte_reg(src)))) {
            return code;
        }

        return std::nullopt;
    }

    constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const GPReg src) {
        if (size == 8) {
            return constants::REX_W | B(src);
        }

        const auto reg = constants::REX | B(src);
        if (reg != constants::REX || (size == 1 && is_special_byte_reg(src))) {
            return reg;
        }

        return std::nullopt;
    }

    constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const Address& src) {
        auto rex = constants::REX | X(src);
        if (const auto base = src.base(); base.has_value()) {
            rex |= B(base.value());
        }

        if (size == 8) {
            return rex | constants::REX_W;
        }

        if (rex != constants::REX) {
            return rex;
        }

        return std::nullopt;
    }

    template<typename Op2, CodeBuffer Buffer>
    requires std::is_same_v<Op2, Address> || std::is_same_v<Op2, GPReg>
    constexpr void emit_op_prologue(Buffer& buffer, const std::uint8_t size, const GPReg op1, const Op2& op2) {
        if (size == 2) {
            add_word_op_size(buffer);
        }

        if (const auto prefix = details::prefix(size, op1, op2); prefix.has_value()) {
            buffer.emit8(prefix.value());
        }
    }

    template<typename Op2, CodeBuffer Buffer>
    requires std::is_same_v<Op2, Address> || std::is_same_v<Op2, GPReg>
    constexpr void emit_op_prologue(Buffer& buffer, const std::uint8_t size, const Op2& op1) {
        if (size == 2) {
            add_word_op_size(buffer);
        }

        if (const auto prefix = details::prefix(size, op1); prefix.has_value()) {
            buffer.emit8(prefix.value());
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, std::uint8_t MODRM, CodeBuffer Buffer>
    constexpr std::optional<Relocation> encode_M(Buffer& buffer, const std::uint8_t size, const Address& addr) {
        if (size == 2) {
            add_word_op_size(buffer);
        }
        auto rex = constants::REX | X(addr);
        if (const auto base = addr.base(); base.has_value()) {
            rex |= B(base.value());
        }
        if (rex != constants::REX) {
            buffer.emit8(rex);
        }

        switch (size) {
            case 1: buffer.emit8(B_CODING); break;
            case 2: [[fallthrough]];
            case 4: [[fallthrough]];
            case 8: buffer.emit8(CODING); break;
            default: die("Invalid size for mov instruction: {}", size);
        }
        return addr.encode(buffer, MODRM);
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_I(Buffer& buffer, const std::uint8_t size, const std::int32_t imm) {
        switch (size) {
            case 1: {
                buffer.emit8(B_CODING);
                buffer.emit8(checked_cast<std::int8_t>(imm));
                break;
            }
            case 2: {
                add_word_op_size(buffer);
                buffer.emit8(CODING);
                buffer.emit16(checked_cast<std::int16_t>(imm));
                break;
            }
            case 4: {
                buffer.emit8(CODING);
                buffer.emit32(checked_cast<std::int32_t>(imm));
                break;
            }
            default: die("Invalid size for pop instruction: {}", size);
        }
    }

    template<std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_O(Buffer& buffer, const std::uint8_t size, const GPReg reg) {
        if (size == 2) {
            add_word_op_size(buffer);
        }
        if (const auto rex = constants::REX | B(reg); rex != constants::REX) {
            buffer.emit8(rex);
        }

        buffer.emit8(CODING + reg3(reg));
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr std::optional<Relocation> encode_MR(Buffer& buffer, const std::uint8_t size, const GPReg src, const Address& dest) {
        emit_op_prologue(buffer, size, src, dest);
        switch (size) {
            case 1: buffer.emit8(B_CODING); break;
            case 2: [[fallthrough]];
            case 4: [[fallthrough]];
            case 8: buffer.emit8(CODING); break;
            default: die("Invalid size for mov instruction: {}", size);
        }
        return dest.encode(buffer, reg3(src));
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr std::optional<Relocation> encode_RM(Buffer& buffer, const std::uint8_t size, const Address& src, const GPReg dest) {
        return encode_MR<B_CODING, CODING>(buffer, size, dest, src);
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_RR(Buffer& buffer, const std::uint8_t size, const GPReg src, const GPReg dest) {
        emit_op_prologue(buffer, size, src, dest);
        switch (size) {
            case 1: buffer.emit8(B_CODING); break;
            case 2: [[fallthrough]];
            case 4: [[fallthrough]];
            case 8: buffer.emit8(CODING); break;
            default: die("Invalid size for mov instruction: {}", size);
        }
        buffer.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, std::uint8_t MODRM, CodeBuffer Buffer>
    constexpr void encode_RI32(Buffer &buffer, const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
        emit_op_prologue(buffer, size, dst);
        switch (size) {
            case 1: {
                buffer.emit8(B_CODING);
                buffer.emit8(0xC0 | MODRM << 3 | reg3(dst));
                buffer.emit8(static_cast<std::int8_t>(imm));
                break;
            }
            case 2: {
                buffer.emit8(CODING);
                buffer.emit8(0xC0 | MODRM << 3 | reg3(dst));
                buffer.emit16(static_cast<std::int16_t>(imm));
                break;
            }
            case 4: [[fallthrough]];
            case 8: {
                buffer.emit8(CODING);
                buffer.emit8(0xC0 | MODRM << 3 | reg3(dst));
                buffer.emit32(static_cast<std::int32_t>(imm));
                break;
            }
            default: die("Invalid size for add instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, CodeBuffer Buffer>
    constexpr void encode_RI64(Buffer& buffer, const std::uint8_t size, const std::int64_t imm, const GPReg dst) {
        emit_op_prologue(buffer, size, dst);
        switch (size) {
            case 1: {
                buffer.emit8(B_CODING | reg3(dst));
                buffer.emit8(checked_cast<std::int8_t>(imm));
                break;
            }
            case 2: {
                buffer.emit8(CODING | reg3(dst));
                buffer.emit16(checked_cast<std::int16_t>(imm));
                break;
            }
            case 4: {
                buffer.emit8(CODING | reg3(dst));
                buffer.emit32(checked_cast<std::int32_t>(imm));
                break;
            }
            case 8: {
                buffer.emit8(CODING | reg3(dst));
                buffer.emit64(imm);
                break;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }

    template<std::uint8_t B_CODING, std::uint8_t CODING, std::uint8_t MODRM, CodeBuffer Buffer>
    constexpr std::optional<Relocation> encode_MI32(Buffer& buffer, const std::uint8_t size, const std::int32_t imm, const Address& dst) {
        emit_op_prologue(buffer, size, dst);
        switch (size) {
            case 1: {
                buffer.emit8(B_CODING);
                const auto reloc = dst.encode(buffer, MODRM);
                buffer.emit8(checked_cast<std::int8_t>(imm));
                return reloc;
            }
            case 2: {
                buffer.emit8(CODING);
                const auto reloc = dst.encode(buffer, MODRM);
                buffer.emit16(checked_cast<std::int16_t>(imm));
                return reloc;
            }
            case 4: [[fallthrough]];
            case 8: {
                buffer.emit8(CODING);
                const auto reloc = dst.encode(buffer, MODRM);
                buffer.emit32(imm);
                return reloc;
            }
            default: die("Invalid size for mov instruction: {}", size);
        }
    }
}