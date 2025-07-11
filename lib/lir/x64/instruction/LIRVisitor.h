#pragma once

#include "lir/x64/lir_frwd.h"

class LIRVisitor {
public:
    virtual ~LIRVisitor() = default;

    virtual void add_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void sub_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void mul_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void div_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void and_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void or_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void xor_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void shl_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void shr_i(const LIRVal& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void parallel_copy(const LIRVal& out, std::span<LIRVal const> inputs) = 0;

    virtual void cmp_i(const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void neg_i(const LIRVal& out, const LIROperand& in) = 0;
    virtual void not_i(const LIRVal& out, const LIROperand& in) = 0;

    virtual void mov_i(const LIRVal& in1, const LIRVal& in2) = 0;
    virtual void copy_i(const LIRVal& out, const LIROperand& in) = 0;

    virtual void jmp(const MachBlock* bb) = 0;
    virtual void je(const MachBlock* on_true, const MachBlock* on_false) = 0;
    virtual void jne(const MachBlock* on_true, const MachBlock* on_false) = 0;
    virtual void jl(const MachBlock* on_true, const MachBlock* on_false) = 0;
    virtual void jle(const MachBlock* on_true, const MachBlock* on_false) = 0;
    virtual void jg(const MachBlock* on_true, const MachBlock* on_false) = 0;
    virtual void jge(const MachBlock* on_true, const MachBlock* on_false) = 0;

    virtual void call(const LIRVal& out, std::span<LIRVal const> args) = 0;
    virtual void vcall(std::span<LIRVal const> args) = 0;
    virtual void icall(const LIRVal& out, const LIRVal& pointer, std::span<LIRVal const> args) = 0;
    virtual void ivcall(const LIRVal& pointer, std::span<LIRVal const> args) = 0;

    virtual void ret(std::span<LIROperand const> ret_values) = 0;
};
