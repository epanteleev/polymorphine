#pragma once
#include "lir/x64/asm/visitors/XUnaryOutVisitor.h"


template<typename TemporalRegStorage, typename AsmEmit>
class CopyFloatEmit final: public XUnaryOutVisitor {
public:
    explicit CopyFloatEmit(const TemporalRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const XVReg& out, const XOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class XUnaryOutVisitor;

    void emit(aasm::XmmRegister out, aasm::XmmRegister in) override {
        unimplemented();
    }

    void emit(aasm::XmmRegister out, const aasm::Address &in) override {
        m_as.movfp(m_size, in, out);
    }

    void emit(const aasm::Address &out, aasm::XmmRegister in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(aasm::XmmRegister out, std::int64_t in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};