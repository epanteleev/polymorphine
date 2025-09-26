#pragma once

#include <cstddef>
#include "asm/x64/Common.h"
#include "asm/x64/reg/XmmRegister.h"

namespace aasm::details {
    template<std::size_t N, std::size_t M, CodeBuffer Buffer>
    requires (N > 0 && M > 0)
    class SSEEncoder final {
    public:
        explicit constexpr SSEEncoder(Buffer& buffer, const std::array<std::uint8_t, M>& prefixes, const std::array<std::uint8_t, N>& opcodes) noexcept:
            m_buffer(buffer),
            m_prefixes(prefixes),
            m_opcodes(opcodes) {}

        constexpr std::optional<Relocation> encode_A(const XmmRegister src, const XmmRegister dest) {
            emit_opcodes(m_prefixes);
            if (const auto prefix = constants::REX | R(dest) | B(src); prefix != constants::REX) {
                m_buffer.emit8(prefix);
            }

            emit_opcodes(m_opcodes);
            m_buffer.emit8(0xC0 | dest.encode() << 3 | src.encode());
            return std::nullopt;
        }

        [[nodiscard]]
        constexpr std::optional<Relocation> encode_A(const Address& src, const XmmRegister dest) {
            emit_opcodes(m_prefixes);
            EncodeUtils::prefix(src, dest);

            emit_opcodes(m_opcodes);
            return src.encode(m_buffer, dest.encode(), 0);
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