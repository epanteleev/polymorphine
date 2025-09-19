#pragma once

#include "lir/x64/asm/AsmModule.h"
#include "lir/x64/module/LIRModule.h"

class Codegen final {
public:
    explicit Codegen(LIRModule &module) noexcept:
        m_module(module) {}

    void run();

    AsmModule result() {
        return AsmModule(std::move(m_symbol_table), std::move(m_assemblers), std::move(m_slots));
    }

private:
    aasm::Slot convert_lir_slot(aasm::SlotType type, const LIRSlot &lir_slot) noexcept;

    LIRModule& m_module;
    aasm::SymbolTable m_symbol_table{}; // Symbol table for the module
    std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer> m_assemblers;
    std::unordered_map<const aasm::Symbol*, aasm::Directive> m_slots;
};