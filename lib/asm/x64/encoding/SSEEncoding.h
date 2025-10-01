#pragma once

#include <cstddef>
#include "asm/x64/Common.h"
#include "asm/x64/reg/XmmReg.h"

namespace aasm::details {
    template<std::size_t N, std::size_t M, CodeBuffer Buffer>
    requires (N > 0 && M > 0)
    class SSEEncoder final {
    public:
        explicit constexpr SSEEncoder(Buffer& buffer, const std::array<std::uint8_t, M>& prefixes, const std::array<std::uint8_t, N>& opcodes) noexcept:
            m_buffer(buffer),
            m_prefixes(prefixes),
            m_opcodes(opcodes) {}

        template<typename Op>
        requires std::is_same_v<Op, Address> || std::is_same_v<Op, XmmReg>
        [[nodiscard]]
        constexpr std::optional<Relocation> encode_A(const Op& src, const XmmReg dest) {
            emit_opcodes(m_prefixes);
            if (const auto prefix = EncodeUtils::prefix(src, dest); prefix.has_value()) {
                m_buffer.emit8(prefix.value());
            }

            emit_opcodes(m_opcodes);
            return EncodeUtils::emit_operands(m_buffer, dest, src);
        }

    private:
        constexpr void emit_opcodes(const std::span<std::uint8_t const>& ops) const {
            for (const auto opcode : ops) m_buffer.emit8(opcode);
        }

        Buffer& m_buffer;
        const std::array<std::uint8_t, M>& m_prefixes;
        const std::array<std::uint8_t, N>& m_opcodes;
    };
}