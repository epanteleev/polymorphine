#pragma once

#include "asm/x64/AsmModule.h"
#include "lir/x64/module/LIRModule.h"

class Codegen final {
public:
    explicit Codegen(LIRModule &module) noexcept:
        m_module(module) {}

    void run();

    aasm::AsmModule result();

private:
    aasm::Slot convert_lir_slot(aasm::SlotType type, const LIRSlot &lir_slot) noexcept;
    void convert_lir_slots(const GlobalData& global_data);

    LIRModule& m_module;
    aasm::SymbolTable m_symbol_table{}; // Symbol table for the module
    std::unordered_map<const aasm::Symbol*, aasm::AsmBuffer> m_assemblers;
    std::unordered_map<const aasm::Symbol*, aasm::Directive> m_slots;
};