#pragma once

#include "asm/global/AssembleSlot.h"
#include "asm/global/Slot.h"
#include "asm/x64/Assembler.h"
#include "asm/x64/Relocation.h"


namespace aasm {
    class Directive final {
    public:
        explicit Directive(const Symbol* symbol, Slot&& slot) noexcept:
            m_symbol(symbol),
            m_slot(std::move(slot)) {}

        [[nodiscard]]
        const Symbol* symbol() const noexcept { return m_symbol; }

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::vector<Relocation> emit(Buffer& buffer) const {
            return details::AssembleSlot::assemble(buffer, m_slot);
        }

        [[nodiscard]]
        const Slot& root() const noexcept { return m_slot; }

        void print_description(std::ostream &os) const;

        friend std::ostream &operator<<(std::ostream &os, const Directive &directive);

    private:
        const Symbol* m_symbol;
        Slot m_slot;
    };

    std::ostream &operator<<(std::ostream &os, const Directive &directive);
}
