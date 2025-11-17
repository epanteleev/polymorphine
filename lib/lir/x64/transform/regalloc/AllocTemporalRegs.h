#pragma once
#include <cstdint>
#include <utility>

#include "asm/asm_frwd.h"
#include "lir/x64/lir_frwd.h"

namespace details {
    class AllocTemporalRegs final {
    public:
        static std::pair<std::uint8_t, std::uint8_t> allocate(aasm::SymbolTable& symbol_tab, const LIRInstructionBase* inst);
    };
}