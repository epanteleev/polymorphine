#pragma once
#include <cstdint>

#include "symbol/Symbol.h"

enum class RelType: std::uint8_t {
    R_X86_64_NONE = 0,
    R_X86_64_64 = 1,                /* word64 S + A. */
    R_X86_64_PC32 = 2,              /* word32 S + A - P */
    R_X86_64_PLT32 = 4,             /* word32 L + A - P */
    R_X86_64_GOTPCREL = 9           /* word32 G + GOT + A - P */
};

namespace aasm {
    class Relocation final {
    public:
        explicit Relocation(const RelType type, const std::uint32_t offset, const std::uint32_t displacement, const Symbol* symbol) noexcept:
            m_type(type),
            m_offset(offset),
            m_displacement(displacement),
            m_symbol(symbol) {}

    private:
        RelType m_type;
        std::uint32_t m_offset;
        std::uint32_t m_displacement;
        const Symbol* m_symbol;
    };
}