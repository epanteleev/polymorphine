#pragma once

#include "asm/asm.h"
#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/module/LIRFuncData.h"


class LIRFunctionCodegen final {
    explicit LIRFunctionCodegen(const LIRFuncData &data, const RegisterAllocation &reg_allocation, const Ordering<LIRBlock>& preorder, aasm::SymbolTable& symbol_table) noexcept:
        m_data(data),
        m_reg_allocation(reg_allocation),
        m_preorder(preorder),
        m_symbol_tab(symbol_table) {}

public:
    void run() {
        setup_basic_block_labels();
        traverse_instructions();
    }

    MasmEmitter result() noexcept {
        return std::move(m_as);
    }

    static LIRFunctionCodegen create(LIRAnalysisPassManager* cache, const LIRFuncData* data, aasm::SymbolTable& symbol_tab) {
        const auto register_allocation = cache->analyze<LinearScanLinuxX64>(data);
        const auto preorder = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
        return LIRFunctionCodegen(*data, *register_allocation, *preorder, symbol_tab);
    }

private:
    void setup_basic_block_labels();
    const TemporalRegs& clobber_reg(const LIRInstructionBase* inst) const;
    void traverse_instructions();

    const LIRFuncData& m_data;
    const RegisterAllocation& m_reg_allocation;
    const Ordering<LIRBlock>& m_preorder;
    aasm::SymbolTable& m_symbol_tab;

    std::unordered_map<const LIRBlock*, aasm::Label> m_bb_labels{};
    MasmEmitter m_as{};
};