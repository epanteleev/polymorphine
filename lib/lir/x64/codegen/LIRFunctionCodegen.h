#pragma once

#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/module/LIRFuncData.h"


class LIRFunctionCodegen final {
    explicit LIRFunctionCodegen(const LIRFuncData &data, const Ordering<LIRBlock>& preorder, aasm::SymbolTable& symbol_table) noexcept:
        m_data(data),
        m_preorder(preorder),
        m_sym_tab(symbol_table) {}

public:
    void run() {
        setup_basic_block_labels();
        traverse_instructions();
    }

    MasmEmitter result() noexcept {
        return std::move(m_as);
    }

    static LIRFunctionCodegen create(LIRAnalysisPassManager* cache, const LIRFuncData* data, aasm::SymbolTable& symbol_tab) {
        const auto preorder = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
        return LIRFunctionCodegen(*data, *preorder, symbol_tab);
    }

private:
    void setup_basic_block_labels();
    void traverse_instructions();

    const LIRFuncData& m_data;
    const Ordering<LIRBlock>& m_preorder;

    std::unordered_map<const LIRBlock*, aasm::Label> m_bb_labels{};
    MasmEmitter m_as{};
    aasm::SymbolTable& m_sym_tab;
};