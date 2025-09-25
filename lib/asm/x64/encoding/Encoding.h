#pragma once

#include "EncodeUtils.h"

namespace aasm::details {
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
                EncodeUtils::add_word_op_size(m_buffer);
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
                EncodeUtils::add_word_op_size(m_buffer);
            }
            if (const auto rex = constants::REX | B(reg); rex != constants::REX) {
                m_buffer.emit8(rex);
            }

            assertion(m_opcodes.size() == 1, "Only single opcode supported for encode_O");
            m_buffer.emit8(m_opcodes[0] + reg.encode());
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_M(const std::uint8_t modrm, const std::uint8_t size, const Address &addr) {
            const auto new_size = size == 8 ? 4 : size; // Avoid REX.W in this contexts by using 32-bit ops
            EncodeUtils::emit_op_prologue(m_buffer, new_size, addr);
            emit_opcodes(size);
            return addr.encode(m_buffer, modrm, 0);
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_M_with_REXW(const std::uint8_t modrm, const std::uint8_t size, const Address& addr) {
            EncodeUtils::emit_op_prologue(m_buffer, size, addr);
            emit_opcodes(size);
            return addr.encode(m_buffer, modrm, 0);
        }

        constexpr void encode_M(const std::uint8_t modrm, const std::uint8_t size, const GPReg reg) {
            if (size == 2) {
                EncodeUtils::add_word_op_size(m_buffer);
            }
            if (auto rex = EncodeUtils::prefix(size, reg); rex.has_value()) {
                m_buffer.emit8(rex.value());
            }

            emit_opcodes(size);
            m_buffer.emit8(0xC0 | modrm << 3 | reg.encode());
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_MI32(std::uint8_t modrm, const std::uint8_t size, const std::int32_t imm, const Address& dst) {
            EncodeUtils::emit_op_prologue(m_buffer, size, dst);
            emit_opcodes(size);
            const auto imm_size = size == 8 ? 4 : size;
            const auto reloc = dst.encode(m_buffer, modrm, imm_size);
            emit_imm_below_i32(size, imm);
            return reloc;
        }

        constexpr void encode_RI64(const std::uint8_t size, const std::int64_t imm, const GPReg dst) {
            EncodeUtils::emit_op_prologue(m_buffer, size, dst);
            assertion(m_opcodes.size() == 1 && m_b_opcodes.size() == 1, "Only single opcode supported for encode_RI64");
            switch (size) {
                case 1: {
                    m_buffer.emit8(m_b_opcodes[0] | dst.encode());
                    m_buffer.emit8(checked_cast<std::int8_t>(imm));
                    break;
                }
                case 2: {
                    m_buffer.emit8(m_opcodes[0] | dst.encode());
                    m_buffer.emit16(checked_cast<std::int16_t>(imm));
                    break;
                }
                case 4: {
                    m_buffer.emit8(m_opcodes[0] | dst.encode());
                    m_buffer.emit32(checked_cast<std::int32_t>(imm));
                    break;
                }
                case 8: {
                    m_buffer.emit8(m_opcodes[0] | dst.encode());
                    m_buffer.emit64(imm);
                    break;
                }
                default: die("Invalid size for instruction: {}", size);
            }
        }

        constexpr void encode_RI32(std::uint8_t modrm, const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
            EncodeUtils::emit_op_prologue(m_buffer, size, dst);
            emit_opcodes(size);
            m_buffer.emit8(0xC0 | modrm << 3 | dst.encode());
            emit_imm_below_i32(size, imm);
        }

        constexpr void encode_MR(const std::uint8_t size, const GPReg src, const GPReg dest) {
            EncodeUtils::emit_op_prologue(m_buffer, size, src, dest);
            emit_opcodes(size);
            m_buffer.emit8(0xC0 | src.encode() << 3 | dest.encode());
        }

        constexpr void encode_MR(const std::uint8_t to_size, const std::uint8_t from_size, const GPReg src, const GPReg dest) {
            EncodeUtils::emit_op_prologue(m_buffer, to_size, from_size, src, dest);
            emit_opcodes(from_size);
            m_buffer.emit8(0xC0 | src.encode() << 3 | dest.encode());
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_MR(const std::uint8_t size, const GPReg src, const Address& dest) {
            EncodeUtils::emit_op_prologue(m_buffer, size, src, dest);
            emit_opcodes(size);
            return dest.encode(m_buffer, src.encode(), 0);
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
            return dest.encode(m_buffer, src.encode(), 0);
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
