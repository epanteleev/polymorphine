#pragma once

#include "mir/mir_frwd.h"

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void accept(Binary *inst) = 0;
    virtual void accept(Unary *inst) = 0;
    virtual void accept(Branch* branch) = 0;
    virtual void accept(CondBranch* cond_branch) = 0;
    virtual void accept(TerminateValueInstruction* inst) = 0;
    virtual void accept(Return* inst) = 0;
    virtual void accept(ReturnValue* inst) = 0;
    virtual void accept(Switch* inst) = 0;
    virtual void accept(VCall* call) = 0;
    virtual void accept(IVCall* call) = 0;
    virtual void accept(PhiInstruction *inst) = 0;
    virtual void accept(Store* store) = 0;
    virtual void accept(Alloc* alloc) = 0;
    virtual void accept(IcmpInstruction *icmp) = 0;
    virtual void accept(GetElementPtr* gep) = 0;
};
