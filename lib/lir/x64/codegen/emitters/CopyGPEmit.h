#pragma once

#include "lir/x64/lir_frwd.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"


class CopyGPEmit final: public GPUnaryOutVisitor {
public:
    static void emit(MasmEmitter& as, const std::uint8_t size, const GPVReg& out, const GPOp& in) {
        CopyGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    explicit CopyGPEmit(MasmEmitter& as, std::uint8_t size) noexcept
        : m_size(size), m_as(as) {}

    void emit(aasm::GPReg out, aasm::GPReg in) override;
    void emit(aasm::GPReg out, const aasm::Address &in) override;
    void emit(const aasm::Address &out, aasm::GPReg in) override;
    void emit(const aasm::Address &out, const aasm::Address &in) override;
    void emit(aasm::GPReg out, std::int64_t in) override;
    void emit(const aasm::Address &out, std::int64_t in) override;

    std::uint8_t m_size;
    MasmEmitter& m_as;
};