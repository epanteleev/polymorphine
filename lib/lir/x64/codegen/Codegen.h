#pragma once

#include "lir/x64/asm/AsmModule.h"
#include "lir/x64/module/LIRModule.h"

class Codegen final {
public:
    explicit Codegen(const LIRModule &module) noexcept:
        m_module(module),
        m_symbol_table(std::make_shared<aasm::SymbolTable>()) {}

    void run();

    AsmModule result() {
        return AsmModule(m_symbol_table, std::move(m_assemblers));
    }

private:
    const LIRModule& m_module;
    std::shared_ptr<aasm::SymbolTable> m_symbol_table; // Symbol table for the module
    std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer> m_assemblers;
};
