#pragma once

#include "lir/x64/lir_frwd.h"

#include "asm/Address.h"


class GPBinaryVisitor {
public:
    virtual ~GPBinaryVisitor() = default;

    virtual void rrr(aasm::GPReg out, aasm::GPReg in1, aasm::GPReg in2) = 0;
    virtual void rrm(aasm::GPReg out, aasm::GPReg in1, const aasm::Address& in2) = 0;
    virtual void rmr(aasm::GPReg out, const aasm::Address& in1, aasm::GPReg in2) = 0;
    virtual void rmm(aasm::GPReg out, const aasm::Address& in1, const aasm::Address& in2) = 0;

    virtual void rri(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) = 0;
    virtual void rir(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) = 0;
    virtual void rii(aasm::GPReg out, std::int32_t in1, std::int32_t in2) = 0;

    virtual void rim(aasm::GPReg out, std::int32_t in1, const aasm::Address& in2) = 0;
    virtual void rmi(aasm::GPReg out, const aasm::Address& in1, std::int32_t in2) = 0;

    virtual void mrr(const aasm::Address& out, aasm::GPReg in1, aasm::GPReg in2) = 0;
    virtual void mrm(const aasm::Address& out, aasm::GPReg in1, const aasm::Address& in2) = 0;
    virtual void mmr(const aasm::Address& out, const aasm::Address& in1, aasm::GPReg in2) = 0;
    virtual void mmm(const aasm::Address& out, const aasm::Address& in1, const aasm::Address& in2) = 0;

    virtual void mri(const aasm::Address& out, aasm::GPReg in1, std::int32_t in2) = 0;
    virtual void mii(const aasm::Address& out, std::int32_t in1, std::int32_t in2) = 0;
    virtual void mmi(const aasm::Address& out, const aasm::Address& in1, std::int32_t in2) = 0;
    virtual void mim(const aasm::Address& out, std::int32_t in1, const aasm::Address& in2) = 0;
    virtual void mir(const aasm::Address& out, std::int32_t in1, aasm::GPReg in2) = 0;

    static void dispatch(GPBinaryVisitor& visitor, const GPVReg& out, const GPOp& in1, const GPOp& in2);
};
