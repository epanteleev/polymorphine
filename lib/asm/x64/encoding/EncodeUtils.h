#pragma once

#include "Prefix.h"
#include "asm/x64/Common.h"
#include "asm/x64/address/Address.h"
#include "asm/x64/reg/Reg.h"

namespace aasm::details {
    class EncodeUtils final {
    public:
        template<CodeBuffer Buffer>
        static constexpr void add_word_op_size(Buffer& buffer) {
            buffer.emit8(constants::PREFIX_OPERAND_SIZE);
        }

        [[nodiscard]]
        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const XmmReg src, const Address& dest) noexcept {
            auto code = R(src) | X(dest);
            if (const auto base = dest.base(); base.has_value()) {
                code |= B(base.value());
            }
            if (size == 8) {
                code |= constants::REX_W;
            } else {
                code |= constants::REX;
            }

            if (code != constants::REX) {
                return code;
            }

            return std::nullopt;
        }

        [[nodiscard]]
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

        template<typename Op>
        requires std::is_same_v<Op, Address> || std::is_same_v<Op, XmmReg>
        [[nodiscard]]
        static constexpr std::optional<std::uint8_t> prefix(const Op& src, const XmmReg dest) noexcept {
            auto code = constants::REX | R(dest);
            if constexpr (std::is_same_v<Op, Address>) {
                code |= X(src);
                if (const auto base = src.base(); base.has_value()) {
                    code |= B(base.value());
                }

            } else if constexpr (std::is_same_v<Op, XmmReg>) {
                code |= B(src);

            } else {
                static_assert(false, "Unsupported type");
                std::unreachable();
            }

            if (code != constants::REX) {
                return code;
            }
            return std::nullopt;
        }

        [[nodiscard]]
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

        [[nodiscard]]
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

        [[nodiscard]]
        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const XmmReg src, const GPReg dest) noexcept {
            auto code = R(src) | B(dest);
            if (size == 8) {
                code |= constants::REX_W;
            } else {
                code |= constants::REX;
            }

            if (code != constants::REX || (size == 1 && is_special_byte_reg(dest))) {
                return code;
            }

            return std::nullopt;
        }

        [[nodiscard]]
        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t size, const GPReg src, const XmmReg dest) noexcept {
            auto code = R(src) | B(dest);
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

        template<typename Reg>
        requires std::same_as<Reg, GPReg> || std::same_as<Reg, XmmReg>
        [[nodiscard]]
        static constexpr std::optional<std::uint8_t> prefix(const std::uint8_t, const Address& src, const Reg dest) noexcept {
            auto code = constants::REX | R(dest) | X(src);
            if (const auto base = src.base(); base.has_value()) {
                code |= B(base.value());
            }
            if (code != constants::REX) {
                return code;
            }
            return std::nullopt;
        }

        [[nodiscard]]
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

        [[nodiscard]]
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

        [[nodiscard]]
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

        template<CodeBuffer Buffer, std::size_t N>
        static constexpr void emit_opcodes(Buffer& buffer, const std::array<std::uint8_t, N>& ops) {
            for (const auto opcode : ops) buffer.emit8(opcode);
        }

        template<typename SRC, typename Op, CodeBuffer Buffer>
        [[nodiscard]]
        static constexpr std::optional<Relocation> emit_operands(Buffer& m_buffer, const SRC& src, const Op& dest) {
            if constexpr (std::is_same_v<Op, GPReg> || std::is_same_v<Op, XmmReg>) {
                m_buffer.emit8(0xC0 | src.encode() << 3 | dest.encode());
                return std::nullopt;

            } else if constexpr (std::is_same_v<Op, Address>) {
                return dest.encode(m_buffer, src.encode(), 0);

            } else {
                static_assert(false, "Unsupported type for encode_MR");
                std::unreachable();
            }
        }
    };
}


