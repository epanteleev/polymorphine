#pragma once

#include "lir/x64/lir_frwd.h"
#include "asm/x64/asm_frwd.h"

class LIRVisitor {
public:
    virtual ~LIRVisitor() = default;

    virtual void gen(const LIRVal& out) = 0;
    virtual void add_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void sub_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void mul_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void div_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void and_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void or_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void xor_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void shl_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void shr_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void setcc_i(const LIRVal& out, aasm::CondType cond_type) = 0;
    virtual void cmov_i(aasm::CondType cond_type, const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void parallel_copy(const LIRVal& out, std::span<LIRVal const> inputs) = 0;

    virtual void movzx_i(const LIRVal& out, const LIROperand& in) = 0;
    virtual void movsx_i(const LIRVal& out, const LIROperand& in) = 0;
    virtual void trunc_i(const LIRVal& out, const LIROperand& in) = 0;

    virtual void cmp_i(const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void neg_i(const LIRVal& out, const LIROperand& in) = 0;
    virtual void not_i(const LIRVal& out, const LIROperand& in) = 0;

    virtual void mov_i(const LIRVal& in1, const LIROperand& in2) = 0;
    virtual void mov_by_idx_i(const LIRVal& pointer, const LIROperand& index, const LIROperand& in) = 0;
    virtual void store_on_stack_i(const LIRVal& pointer, const LIROperand& index, const LIROperand& value) = 0;
    virtual void store_i(const LIRVal& pointer, const LIROperand& value) = 0;
    virtual void up_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) = 0;
    virtual void down_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) = 0;
    virtual void prologue(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) = 0;
    virtual void epilogue(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) = 0;

    virtual void copy_i(const LIRVal& out, const LIROperand& in) = 0;
    virtual void load_i(const LIRVal& out, const LIRVal& pointer) = 0;
    virtual void load_by_idx_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) = 0;
    virtual void load_from_stack_i(const LIRVal& out, const LIRVal& pointer, const LIROperand& index) = 0;
    virtual void lea_i(const LIRVal& out, const LIRVal& pointer, const LIROperand& index) = 0;

    virtual void jmp(const LIRBlock* bb) = 0;
    virtual void jcc(aasm::CondType cond_type, const LIRBlock* on_true, const LIRBlock* on_false) = 0;

    virtual void call(const LIRVal& out, std::string_view name, std::span<LIRVal const> args, LIRLinkage linkage) = 0;
    virtual void vcall(std::span<LIRVal const> args) = 0;
    virtual void icall(const LIRVal& out, const LIRVal& pointer, std::span<LIRVal const> args) = 0;
    virtual void ivcall(const LIRVal& pointer, std::span<LIRVal const> args) = 0;

    virtual void ret(std::span<LIRVal const> ret_values) = 0;
};
