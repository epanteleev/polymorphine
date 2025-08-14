#pragma once

#include "asm/asm.h"
#include "lir/x64/analysis/regalloc/RegisterAllocation.h"
#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/instruction/LIRVisitor.h"
#include "lir/x64/operand/LIRVal.h"


class LIRInstructionCodegen final: public LIRVisitor {
public:
    LIRInstructionCodegen(const TemporalRegs& clobber_regs,
            const RegisterAllocation &reg_allocation,
            aasm::SymbolTable& symbol_table,
            const std::unordered_map<const LIRBlock*, aasm::Label>& bb_labels,
            MasmEmitter& as) noexcept:
        m_clobber_regs(clobber_regs),
        m_reg_allocation(reg_allocation),
        m_symbol_tab(symbol_table),
        m_bb_labels(bb_labels),
        m_as(as) {}

    void run(const LIRInstructionBase* inst) {
        const_cast<LIRInstructionBase*>(inst)->visit(*this);
    }

private:
    [[nodiscard]]
    GPOp convert_to_gp_op(const LIROperand &val) const;

    [[nodiscard]]
    aasm::GPReg convert_to_gp_reg(const LIRVal &val) const;

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

    void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {

    }

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

    void store_i(const LIRVal &pointer, const LIROperand &value) override;

    void up_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) override;

    void down_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) override;

    void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override;

    void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override;

    void copy_i(const LIRVal &out, const LIROperand &in) override;

    void load_i(const LIRVal &out, const LIRVal &pointer) override;

    void jmp(const LIRBlock *bb) override;

    void jcc(aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override;

    void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {

    }

    void call(const LIRVal &out, std::string_view name, std::span<LIRVal const> args, LIRLinkage) override;

    void vcall(std::span<LIRVal const> args) override {

    }

    void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {

    }

    void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {

    }

    void ret(std::span<LIRVal const> ret_values) override;

    const TemporalRegs& m_clobber_regs;
    const RegisterAllocation& m_reg_allocation;
    aasm::SymbolTable& m_symbol_tab;
    const std::unordered_map<const LIRBlock*, aasm::Label>& m_bb_labels;
    MasmEmitter& m_as;
};
