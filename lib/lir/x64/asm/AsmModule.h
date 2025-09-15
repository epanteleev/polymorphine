#pragma once

#include <unordered_map>
#include <memory>
#include <expected>

#include "MasmEmitter.h"
#include "asm/symbol/SymbolTable.h"
#include "lir/x64/global/NamedSlot.h"

class AsmModule final {
public:
    explicit AsmModule(std::shared_ptr<aasm::SymbolTable> symbol_table,
        std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer>&& modules,
        std::unordered_map<const aasm::Symbol*, NamedSlot>&& slots) noexcept:
        m_symbol_table(std::move(symbol_table)),
        m_modules(std::move(modules)),
        m_slots(std::move(slots)) {}

    const auto& assembler() const noexcept {
        return m_modules;
    }

    const auto& globals() const noexcept {
        return m_slots;
    }

    std::shared_ptr<aasm::SymbolTable> symbol_table() const noexcept {
        return m_symbol_table;
    }

    std::expected<const aasm::AsmBuffer*, Error> function(const std::string& name) const noexcept {
        const auto symbol = m_symbol_table->find(name);
        if (!symbol.has_value()) {
            return std::unexpected(Error::NotFoundError);
        }
        const auto it = m_modules.find(symbol.value());
        if (it == m_modules.end()) {
            return std::unexpected(Error::NotFoundError);
        }

        return &it->second;
    }

    friend std::ostream& operator<<(std::ostream &os, const AsmModule &module);

private:
    std::shared_ptr<aasm::SymbolTable> m_symbol_table; // Symbol table for the module
    std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer> m_modules;
    std::unordered_map<const aasm::Symbol*, NamedSlot> m_slots;
};

std::ostream & operator<<(std::ostream &os, const AsmModule &module);