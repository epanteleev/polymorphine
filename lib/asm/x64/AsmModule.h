#pragma once

#include <unordered_map>
#include <memory>
#include <expected>

#include "asm/symbol/SymbolTable.h"
#include "asm/global/Directive.h"
#include "asm/x64/AsmBuffer.h"

namespace aasm {
    class AsmModule final {
    public:
        explicit AsmModule(SymbolTable&& symbol_table,
            std::unordered_map<const Symbol*, AsmBuffer>&& asm_buffers,
            std::unordered_map<const Symbol*, Directive>&& slots) noexcept:
            m_symbol_table(std::move(symbol_table)),
            m_asm_buffers(std::move(asm_buffers)),
            m_global_slots(std::move(slots)) {}

        std::expected<const AsmBuffer*, Error> function(const std::string& name) const noexcept {
            const auto symbol = m_symbol_table.find(name);
            if (!symbol.has_value()) {
                return std::unexpected(Error::NotFoundError);
            }
            const auto it = m_asm_buffers.find(symbol.value());
            if (it == m_asm_buffers.end()) {
                return std::unexpected(Error::NotFoundError);
            }

            return &it->second;
        }

        friend std::ostream& operator<<(std::ostream &os, const AsmModule &module);

        SymbolTable m_symbol_table; // Symbol table for the module.
        std::unordered_map<const Symbol*, AsmBuffer> m_asm_buffers; // Asm buffers for the module.
        std::unordered_map<const Symbol*, Directive> m_global_slots; // Global values.
    };

    std::ostream & operator<<(std::ostream &os, const AsmModule &module);
}