#pragma once

#include <cstdint>
#include <iosfwd>

#include "asm/symbol/Symbol.h"

namespace aasm {
    enum class RelocationType: std::uint8_t {
        ADDR_PLT,
        ADDR_GOT,
    };

    class AddressLiteral final {
    public:
        explicit AddressLiteral(const RelocationType reloc, const Symbol* symbol, const std::uint32_t displacement) noexcept:
            m_symbol(symbol),
            m_displacement(displacement),
            m_relocation_type(reloc) {}

        friend std::ostream &operator<<(std::ostream &os, const AddressLiteral &address);

    private:
        const Symbol* m_symbol;
        std::uint32_t m_displacement;
        RelocationType m_relocation_type;
    };

    inline std::ostream &operator<<(std::ostream &os, const AddressLiteral &address) {
        return os << address.m_symbol->name();
    }
}