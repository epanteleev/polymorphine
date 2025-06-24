#pragma once

#include "mach_frwd.h"

class LIRVisitor {
public:
    virtual ~LIRVisitor() = default;

    virtual void add_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void sub_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void mul_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void div_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void and_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void or_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void xor_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void shl_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;
    virtual void shr_i(const VReg& out, const LIROperand& in1, const LIROperand& in2) = 0;

    virtual void neg_i(const VReg& out, const LIROperand& in) = 0;
    virtual void not_i(const VReg& out, const LIROperand& in) = 0;

    virtual void mov_i(const VReg& out, const VReg& in) = 0;

    virtual void jmp(MachBlock* bb) = 0;
    virtual void jz(MachBlock* on_true, MachBlock* on_false) = 0;
    virtual void je(MachBlock* on_true, MachBlock* on_false) = 0;
    virtual void jl(MachBlock* on_true, MachBlock* on_false) = 0;
    virtual void jg(MachBlock* on_true, MachBlock* on_false) = 0;

    virtual void call(const VReg& out, const LIROperand& callee, std::span<const LIROperand> args) = 0;

    virtual void push(const LIROperand& value) = 0;
    virtual void pop(const VReg& out) = 0;

    virtual void ret() = 0;

    virtual void accept(LIRInstruction *inst) = 0;

};
