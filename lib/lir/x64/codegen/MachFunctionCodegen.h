#pragma once

#include <span>

#include "asm/symbol/SymbolTable.h"
#include "lir/x64/analysis/Analysis.h"
#include "lir/x64/asm/GPOp.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/instruction/LIRVisitor.h"
#include "lir/x64/module/LIRFuncData.h"


class MachFunctionCodegen final: public LIRVisitor {
    explicit MachFunctionCodegen(const LIRFuncData &data,
                                 const RegisterAllocation &reg_allocation, const Ordering<LIRBlock>& preorder, aasm::SymbolTable& symbol_table):
        m_data(data),
        m_reg_allocation(reg_allocation),
        m_preorder(preorder),
        m_symbol_tab(symbol_table) {}

public:
    void run() {
        setup_basic_block_labels();
        emit_prologue();
        traverse_instructions();
    }

    MasmEmitter result() noexcept {
        return std::move(m_as);
    }

    static MachFunctionCodegen create(LIRAnalysisPassManager* cache, const LIRFuncData* data, aasm::SymbolTable& symbol_tab) {
        const auto register_allocation = cache->analyze<LinearScanLinuxX64>(data);
        const auto preorder = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
        return MachFunctionCodegen(*data, *register_allocation, *preorder, symbol_tab);
    }

private:
    void setup_basic_block_labels() {
        for (const auto& bb: m_preorder) {
            if (bb == m_data.first()) {
                // Skip the first basic block, it does not need a label.
                continue;
            }

            const auto label = m_as.create_label();
            m_bb_labels.emplace(bb, label);
        }
    }

    void emit_prologue() {
        if (m_reg_allocation.local_area_size() == 0) {
            return;
        }

        m_as.push(8, aasm::rbp);
        m_as.copy(8, aasm::rsp, aasm::rbp);
        m_as.add(8, -m_reg_allocation.local_area_size(), aasm::rsp);
    }

    void emit_epilogue() {
        if (m_reg_allocation.local_area_size() == 0) {
            return;
        }

        m_as.leave();
    }

    void traverse_instructions() {
        for (const auto& bb: m_preorder) {
            if (bb != m_data.first()) {
                const auto label = m_bb_labels.at(bb);
                m_as.set_label(label);
            }

            for (auto& inst: bb->instructions()) inst.visit(*this);
        }
    }

    [[nodiscard]]
    GPOp convert_to_gp_op(const LIROperand &val) const;

    [[nodiscard]]
    aasm::GPReg convert_to_gp_reg(const LIRVal &val) const;

    void gen(const LIRVal &out) override {}

    void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override;

    void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void div_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

    void setcc_i(const LIRVal &out, LIRCondType cond_type) override;

    void cmp_i(const LIROperand &in1, const LIROperand &in2) override;

    void neg_i(const LIRVal &out, const LIROperand &in) override {

    }

    void not_i(const LIRVal &out, const LIROperand &in) override {

    }

    void mov_i(const LIRVal &in1, const LIROperand &in2) override;

    void store_i(const LIRVal &pointer, const LIROperand &value) override;

    void copy_i(const LIRVal &out, const LIROperand &in) override;

    void load_i(const LIRVal &out, const LIRVal &pointer) override;

    void jmp(const LIRBlock *bb) override;

    void jcc(LIRCondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override;

    void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {

    }

    void call(const LIRVal &out, std::string_view name, std::span<LIRVal const> args) override ;

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
};
