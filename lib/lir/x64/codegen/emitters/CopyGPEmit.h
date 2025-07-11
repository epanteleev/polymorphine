#pragma once

#include "lir/x64/asm/MacroAssembler.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"


class CopyGPEmit final: public GPUnaryVisitor {
public:
    static void emit(MacroAssembler& as, std::uint8_t size, const GPVReg& out, const GPOp& in) {
        CopyGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    explicit CopyGPEmit(MacroAssembler& as, std::uint8_t size) noexcept
        : m_size(size), m_as(as) {}

    void rr(aasm::GPReg out, aasm::GPReg in) override;

    void rm(aasm::GPReg out, const aasm::Address &in) override;

    void mr(const aasm::Address &out, aasm::GPReg in) override;

    void mm(const aasm::Address &out, const aasm::Address &in) override;

    void ri(aasm::GPReg out, std::int64_t in) override;

    void mi(const aasm::Address &out, std::int64_t in) override;

    std::uint8_t m_size;
    MacroAssembler& m_as;
};