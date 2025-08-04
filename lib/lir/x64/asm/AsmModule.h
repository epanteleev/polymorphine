#pragma once

#include <unordered_map>
#include <iomanip>
#include <memory>

#include "MasmEmitter.h"
#include "asm/symbol/SymbolTable.h"

class AsmModule final {
public:
    explicit AsmModule(std::shared_ptr<aasm::SymbolTable> symbol_table, std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer>&& modules) noexcept:
        m_symbol_table(std::move(symbol_table)),
        m_modules(std::move(modules)) {}

    const auto& assembler() const noexcept {
        return m_modules;
    }

    std::shared_ptr<aasm::SymbolTable> symbol_table() const noexcept {
        return m_symbol_table;
    }

    friend std::ostream& operator<<(std::ostream &os, const AsmModule &module);

private:
    std::shared_ptr<aasm::SymbolTable> m_symbol_table; // Symbol table for the module
    std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer> m_modules;
};

std::ostream & operator<<(std::ostream &os, const AsmModule &module);