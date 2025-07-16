#pragma once

#include "lir/x64/asm/AsmEmitter.h"
#include "lir/x64/asm/visitors/GPBinaryVisitor.h"


class AddIntEmit final: public GPBinaryVisitor {
public:
    static void emit(AsmEmitter& as, const std::uint8_t size, const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        AddIntEmit emitter(as, size);
        dispatch(emitter, out, in1, in2);
    }

private:
    explicit AddIntEmit(AsmEmitter& as, const std::uint8_t size) noexcept
        : m_size(size), m_as(as) {}

    void rrr(aasm::GPReg out, aasm::GPReg in1, aasm::GPReg in2) override;

    void rrm(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override;

    void rmr(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) override;

    void rmm(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) override;

    void rri(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) override;

    void rir(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) override;

    void rii(aasm::GPReg out, std::int32_t in1, std::int32_t in2) override;

    void rim(aasm::GPReg out, std::int32_t in1, const aasm::Address &in2) override;

    void rmi(aasm::GPReg out, const aasm::Address &in1, std::int32_t in2) override;

    void mrr(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) override;

    void mrm(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) override;

    void mmr(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) override;

    void mmm(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override;

    void mri(const aasm::Address &out, aasm::GPReg in1, std::int32_t in2) override;

    void mii(const aasm::Address &out, std::int32_t in1, std::int32_t in2) override;

    void mmi(const aasm::Address &out, const aasm::Address &in1, std::int32_t in2) override;

    void mim(const aasm::Address &out, std::int32_t in1, const aasm::Address &in2) override;

    void mir(const aasm::Address &out, std::int32_t in1, aasm::GPReg in2) override;

    std::uint8_t m_size;
    AsmEmitter& m_as;
};
