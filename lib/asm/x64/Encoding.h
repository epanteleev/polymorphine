#pragma once

#include "Common.h"
#include "address/Address.h"
#include "reg/Register.h"

namespace aasm::details {
    class EncodeUtils final {
    public:
        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const GPReg src, const Address& dest) noexcept {
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

        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t to_size, const std::uint8_t from_size, const GPReg src, const Address& dest) noexcept {
            auto code = R(src) | X(dest);
            if (const auto base = dest.base(); base.has_value()) {
                code |= B(base.value());
            }
            if (to_size == 8) {
                code |= constants::REX_W;
            } else {
                code |= constants::REX;
            }

            if (code != constants::REX || (from_size == 1 && is_special_byte_reg(src))) {
                return code;
            }

            return std::nullopt;
        }

        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const GPReg src, const GPReg dest) noexcept {
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

        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t to_size, const std::uint8_t from_size, const GPReg src, const GPReg dest) noexcept {
            auto code = R(src) | B(dest);
            if (to_size == 8) {
                code |= constants::REX_W;
            } else {
                code |= constants::REX;
            }

            if (code != constants::REX || (from_size == 1 && is_special_byte_reg(dest))) {
                return code;
            }

            return std::nullopt;
        }

        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const GPReg src) {
            if (size == 8) {
                return constants::REX_W | B(src);
            }

            const auto reg = constants::REX | B(src);
            if (reg != constants::REX || (size == 1 && is_special_byte_reg(src))) {
                return reg;
            }

            return std::nullopt;
        }

        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const Address& src) {
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
        static constexpr void emit_op_prologue(Buffer& buffer, const std::uint8_t size, const GPReg op1, const Op2& op2) {
            if (size == 2) {
                add_word_op_size(buffer);
            }

            if (const auto pr = prefix(size, op1, op2); pr.has_value()) {
                buffer.emit8(pr.value());
            }
        }

        template<typename Op2, CodeBuffer Buffer>
        requires std::is_same_v<Op2, Address> || std::is_same_v<Op2, GPReg>
        static constexpr void emit_op_prologue(Buffer& buffer, const std::uint8_t to_size, const std::uint8_t from_size, const GPReg op1, const Op2& op2) {
            if (to_size == 2) {
                add_word_op_size(buffer);
            }

            if (const auto pr = prefix(to_size, from_size, op1, op2); pr.has_value()) {
                buffer.emit8(pr.value());
            }
        }

        template<typename Op2, CodeBuffer Buffer>
        requires std::is_same_v<Op2, Address> || std::is_same_v<Op2, GPReg>
        static constexpr void emit_op_prologue(Buffer& buffer, const std::uint8_t size, const Op2& op1) {
            if (size == 2) {
                add_word_op_size(buffer);
            }

            if (const auto pr = prefix(size, op1); pr.has_value()) {
                buffer.emit8(pr.value());
            }
        }
    };

    template<std::size_t N, CodeBuffer Buffer>
    requires (N > 0)
    class Encoder final {
    public:
        explicit constexpr Encoder(Buffer& buffer, const std::array<std::uint8_t, N>& b_opcodes, const std::array<std::uint8_t, N>& opcodes) noexcept:
            m_buffer(buffer),
            m_b_opcodes(b_opcodes),
            m_opcodes(opcodes) {}

        constexpr void encode_I(const std::uint8_t size, const std::int32_t imm) const {
            if (size == 2) {
                add_word_op_size(m_buffer);
            }
            emit_opcodes(size);
            switch (size) {
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 4: emit_imm_below_i32(size, imm); break;
                default: die("Invalid size for instruction: {}", size);
            }
        }

        constexpr void encode_O(const std::uint8_t size, const GPReg reg) {
            if (size == 2) {
                add_word_op_size(m_buffer);
            }
            if (const auto rex = constants::REX | B(reg); rex != constants::REX) {
                m_buffer.emit8(rex);
            }

            assertion(m_opcodes.size() == 1, "Only single opcode supported for encode_O");
            m_buffer.emit8(m_opcodes[0] + reg3(reg));
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_M(const std::uint8_t modrm, const std::uint8_t size, const Address &addr) {
            const auto new_size = size == 8 ? 4 : size; // Avoid REX.W in this contexts by using 32-bit ops
            EncodeUtils::emit_op_prologue(m_buffer, new_size, addr);
            emit_opcodes(size);
            return addr.encode(m_buffer, modrm);
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_M_with_REXW(const std::uint8_t modrm, const std::uint8_t size, const Address& addr) {
            EncodeUtils::emit_op_prologue(m_buffer, size, addr);
            emit_opcodes(size);
            return addr.encode(m_buffer, modrm);
        }

        constexpr void encode_M(const std::uint8_t modrm, const std::uint8_t size, const GPReg reg) {
            if (size == 2) {
                add_word_op_size(m_buffer);
            }
            if (auto rex = EncodeUtils::prefix(size, reg); rex.has_value()) {
                m_buffer.emit8(rex.value());
            }

            emit_opcodes(size);
            m_buffer.emit8(0xC0 | modrm << 3 | reg3(reg));
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_MI32(std::uint8_t modrm, const std::uint8_t size, const std::int32_t imm, const Address& dst) {
            EncodeUtils::emit_op_prologue(m_buffer, size, dst);
            emit_opcodes(size);
            const auto reloc = dst.encode(m_buffer, modrm);
            emit_imm_below_i32(size, imm);
            return reloc;
        }

        constexpr void encode_RI64(const std::uint8_t size, const std::int64_t imm, const GPReg dst) {
            EncodeUtils::emit_op_prologue(m_buffer, size, dst);
            assertion(m_opcodes.size() == 1 && m_b_opcodes.size() == 1, "Only single opcode supported for encode_RI64");
            switch (size) {
                case 1: {
                    m_buffer.emit8(m_b_opcodes[0] | reg3(dst));
                    m_buffer.emit8(checked_cast<std::int8_t>(imm));
                    break;
                }
                case 2: {
                    m_buffer.emit8(m_opcodes[0] | reg3(dst));
                    m_buffer.emit16(checked_cast<std::int16_t>(imm));
                    break;
                }
                case 4: {
                    m_buffer.emit8(m_opcodes[0] | reg3(dst));
                    m_buffer.emit32(checked_cast<std::int32_t>(imm));
                    break;
                }
                case 8: {
                    m_buffer.emit8(m_opcodes[0] | reg3(dst));
                    m_buffer.emit64(imm);
                    break;
                }
                default: die("Invalid size for instruction: {}", size);
            }
        }

        constexpr void encode_RI32(std::uint8_t modrm, const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
            EncodeUtils::emit_op_prologue(m_buffer, size, dst);
            emit_opcodes(size);
            m_buffer.emit8(0xC0 | modrm << 3 | reg3(dst));
            emit_imm_below_i32(size, imm);
        }

        constexpr void encode_MR(const std::uint8_t size, const GPReg src, const GPReg dest) {
            EncodeUtils::emit_op_prologue(m_buffer, size, src, dest);
            emit_opcodes(size);
            m_buffer.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
        }

        constexpr void encode_MR(const std::uint8_t to_size, const std::uint8_t from_size, const GPReg src, const GPReg dest) {
            EncodeUtils::emit_op_prologue(m_buffer, to_size, from_size, src, dest);
            emit_opcodes(from_size);
            m_buffer.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_MR(const std::uint8_t size, const GPReg src, const Address& dest) {
            EncodeUtils::emit_op_prologue(m_buffer, size, src, dest);
            emit_opcodes(size);
            return dest.encode(m_buffer, reg3(src));
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_RM(const std::uint8_t size, const Address& src, const GPReg dest) {
            return encode_MR(size, dest, src);
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_MR(const std::uint8_t to_size, const std::uint8_t from_size, const GPReg src, const Address& dest) {
            if (from_size == to_size) {
                die("From and to size must be different for extension");
            }

            EncodeUtils::emit_op_prologue(m_buffer, to_size, src, dest);
            emit_opcodes(from_size);
            return dest.encode(m_buffer, reg3(src));
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_RM(const std::uint8_t to_size, const std::uint8_t from_size, const Address& src, const GPReg dest) {
            return encode_MR(to_size, from_size, dest, src);
        }

    private:
        constexpr void emit_opcodes(const std::array<std::uint8_t, N>& ops) const {
            for (const auto opcode : ops) m_buffer.emit8(opcode);
        }

        constexpr void emit_opcodes(const std::uint8_t size) const {
            switch (size) {
                case 1: emit_opcodes(m_b_opcodes); break;
                case 2: [[fallthrough]];
                case 4: [[fallthrough]];
                case 8: emit_opcodes(m_opcodes); break;
                default: die("Invalid size for instruction: {}", size);
            }
        }

        constexpr void emit_imm_below_i32(const std::uint8_t size, const std::int32_t imm) const {
            switch (size) {
                case 1: m_buffer.emit8(checked_cast<std::int8_t>(imm)); break;
                case 2: m_buffer.emit16(checked_cast<std::int16_t>(imm)); break;
                case 4: [[fallthrough]];
                case 8: m_buffer.emit32(imm); break;
                default: die("Invalid size for instruction: {}", size);
            }
        }

        Buffer& m_buffer;
        const std::array<std::uint8_t, N>& m_b_opcodes;
        const std::array<std::uint8_t, N>& m_opcodes;
    };
}