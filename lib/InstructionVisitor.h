#pragma once

#include "ir_frwd.h"

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void accept(Binary *inst) = 0;
    virtual void accept(Unary *inst) = 0;
    virtual void accept(TerminateInstruction *inst) = 0;
    virtual void accept(PhiInstruction *inst) = 0;
};