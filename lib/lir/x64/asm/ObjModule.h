#pragma once

#include <unordered_map>
#include <iomanip>

#include "MasmEmitter.h"
#include "asm/symbol/SymbolTable.h"

class ObjModule final {
public:
    explicit ObjModule(aasm::SymbolTable&& symbol_table, std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer>&& modules) noexcept:
        m_symbol_table(std::move(symbol_table)),
        m_modules(std::move(modules)) {}

    const auto& assembler() const noexcept {
        return m_modules;
    }

    aasm::SymbolTable&& symbol_table() noexcept {
        return std::move(m_symbol_table);
    }

    friend std::ostream& operator<<(std::ostream &os, const ObjModule &module);

private:
    aasm::SymbolTable m_symbol_table; // Symbol table for the module
    std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer> m_modules;
};

inline std::ostream & operator<<(std::ostream &os, const ObjModule &module) {
    for (const auto& [name, masm]: module.m_modules) {
        os << *name << ':' << std::endl;
        os << std::setfill(' ') << std::setw(4) << masm << std::endl << std::endl;
    }

    return os;
}
