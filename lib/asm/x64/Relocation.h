#pragma once
#include <cstdint>

#include "asm/symbol/Symbol.h"

enum class RelType: std::uint8_t {
    X86_64_NONE = 0,
    X86_64_64 = 1,                /* word64 S + A. */
    X86_64_PC32 = 2,              /* word32 S + A - P */
    X86_64_PLT32 = 4,             /* word32 L + A - P */
    X86_64_GLOB_DAT = 6,          /* word64 S */
    X86_64_GOTPCREL = 9           /* word32 G + GOT + A - P */
};

namespace aasm {
    class Relocation final {
    public:
        explicit Relocation(const RelType type, const std::int32_t offset, const std::int32_t displacement, const Symbol* symbol) noexcept:
            m_type(type),
            m_offset(offset),
            m_displacement(displacement),
            m_symbol(symbol) {}

        /** Returns the symbol associated with the relocation. */
        [[nodiscard]]
        const Symbol* symbol() const noexcept { return m_symbol; }

        [[nodiscard]]
        std::string_view symbol_name() const noexcept { return m_symbol->name(); }

        /** Returns the offset within the code buffer where the relocation needs to be applied. */
        [[nodiscard]]
        std::int32_t offset() const noexcept { return m_offset; }

        /** Returns the displacement used in the relocation calculation. */
        [[nodiscard]]
        std::int32_t displacement() const noexcept { return m_displacement; }

        [[nodiscard]]
        RelType type() const noexcept { return m_type; }

    private:
        RelType m_type;
        std::int32_t m_offset;
        std::int32_t m_displacement;
        const Symbol* m_symbol;
    };
}