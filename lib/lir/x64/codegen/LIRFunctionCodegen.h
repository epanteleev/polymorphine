#pragma once

#include "asm/x64/asm.h"
#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/module/LIRFuncData.h"


class LIRFunctionCodegen final: public LIRVisitor {
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
    const TemporalRegs& temporal_reg(const LIRInstructionBase* inst) const;
    void traverse_instructions();

    [[nodiscard]]
    GPOp convert_to_gp_op(const LIROperand &val) const;

    void gen(const LIRVal &out) override {}

    void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override;

    void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override;

    void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void div_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override;

    void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void setcc_i(const LIRVal &out, aasm::CondType cond_type) override;

    void cmov_i(aasm::CondType cond_type, const LIRVal& out, const LIROperand& in1, const LIROperand& in2) override;

    void cmp_i(const LIROperand &in1, const LIROperand &in2) override;

    void neg_i(const LIRVal &out, const LIROperand &in) override {

    }

    void not_i(const LIRVal &out, const LIROperand &in) override {

    }

    void mov_i(const LIRVal &in1, const LIROperand &in2) override;

    void mov_by_idx_i(const LIRVal &out, const LIROperand &index, const LIROperand &in2) override;

    void load_by_idx_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override;

    void store_i(const LIRVal &pointer, const LIROperand &value) override;

    void up_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) override;

    void down_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) override;

    void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override;

    void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override;

    void copy_i(const LIRVal &out, const LIROperand &in) override;

    void load_i(const LIRVal &out, const LIRVal &pointer) override;

    void lea_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override;

    void jmp(const LIRBlock *bb) override;

    void jcc(aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override;

    void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {}

    void movsx_i(const LIRVal &out, const LIROperand &in) override;
    void movzx_i(const LIRVal &out, const LIROperand &in) override;
    void trunc_i(const LIRVal &out, const LIROperand &in) override;

    void call(const LIRVal &out, std::string_view name, std::span<LIRVal const> args, LIRLinkage) override;

    void vcall(std::span<LIRVal const> args) override {

    }

    void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {

    }

    void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {

    }

    void ret(std::span<LIRVal const> ret_values) override;
    
    const LIRFuncData& m_data;
    const RegisterAllocation& m_reg_allocation;
    const Ordering<LIRBlock>& m_preorder;
    aasm::SymbolTable& m_symbol_tab;

    std::unordered_map<const LIRBlock*, aasm::Label> m_bb_labels{};
    MasmEmitter m_as{};
    const LIRBlock* m_next{};
    const LIRInstructionBase* m_current_inst{};
};