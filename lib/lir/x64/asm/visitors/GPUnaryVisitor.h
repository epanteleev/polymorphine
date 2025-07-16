#pragma once

#include "asm/Address.h"

#include "lir/x64/lir_frwd.h"


class GPUnaryVisitor {
public:
    virtual ~GPUnaryVisitor() = default;

    virtual void rr(aasm::GPReg out, aasm::GPReg in) = 0;
    virtual void rm(aasm::GPReg out, const aasm::Address& in) = 0;
    virtual void mr(const aasm::Address& out, aasm::GPReg in) = 0;
    virtual void mm(const aasm::Address& out, const aasm::Address& in) = 0;
    virtual void ri(aasm::GPReg out, std::int64_t in) = 0;
    virtual void mi(const aasm::Address& out, std::int64_t in) = 0;

    static void dispatch(GPUnaryVisitor& visitor, const GPVReg& out, const GPOp& in);
};


