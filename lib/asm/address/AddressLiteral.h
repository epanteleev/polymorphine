#pragma once

#include <cstdint>
#include <iosfwd>

#include "asm/symbol/Symbol.h"
#include "asm/Common.h"
#include "asm/Relocation.h"

namespace aasm {
    class AddressLiteral final {
    public:
        explicit AddressLiteral(const Symbol* symbol, const std::uint32_t displacement) noexcept:
            m_symbol(symbol),
            m_displacement(displacement) {}

        friend std::ostream &operator<<(std::ostream &os, const AddressLiteral &address);

        template<CodeBuffer Buffer>
        constexpr Relocation encode(Buffer &buffer, const std::uint32_t modrm_pattern) const {
            static constexpr std::uint8_t MODRM = 0x05; // ModR/M byte for direct addressing
            buffer.emit8((modrm_pattern & 0x7) << 3 | MODRM);
            buffer.emit32(0);

            return Relocation(RelType::R_X86_64_GOTPCREL, buffer.size(), m_displacement, m_symbol);
        }

        [[nodiscard]]
        const Symbol* symbol() const noexcept {
            return m_symbol;
        }

    private:
        const Symbol* m_symbol;
        std::uint32_t m_displacement;
    };

    inline std::ostream &operator<<(std::ostream &os, const AddressLiteral &address) {
        return os << address.m_symbol->name();
    }
}
