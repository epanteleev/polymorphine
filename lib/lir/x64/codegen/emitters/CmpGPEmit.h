#pragma once

#include "lir/x64/lir_frwd.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"

class AsmEmitter;

class CmpGPEmit final: public GPUnaryVisitor {
public:
    static void emit(AsmEmitter& as, std::uint8_t size, const GPOp& out, const GPOp& in) {
        CmpGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryVisitor;

    explicit CmpGPEmit(AsmEmitter& as, std::uint8_t size) noexcept
        : m_size(size), m_as(as) {}

    void emit(aasm::GPReg out, aasm::GPReg in) override;
    void emit(aasm::GPReg out, const aasm::Address &in) override;
    void emit(const aasm::Address &out, aasm::GPReg in) override;
    void emit(const aasm::Address &out, const aasm::Address &in) override;
    void emit(aasm::GPReg out, std::int64_t in) override;
    void emit(const aasm::Address &out, std::int64_t in) override;
    void emit(std::int64_t in1, std::int64_t in2) override;
    void emit(std::int64_t in1, aasm::GPReg in2) override;
    void emit(std::int64_t in1, const aasm::Address &in2) override;

    std::uint8_t m_size;
    AsmEmitter& m_as;
};
