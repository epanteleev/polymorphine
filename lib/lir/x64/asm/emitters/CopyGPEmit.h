#pragma once

#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"

class CopyGPEmit final: public GPUnaryOutVisitor {
public:
    static void apply(MasmEmitter& as, const std::uint8_t size, const GPVReg& out, const GPOp& in) {
        CopyGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    explicit CopyGPEmit(MasmEmitter& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as) {}

    void emit(const aasm::GPReg out, const aasm::GPReg in) override  {
        m_as.copy(m_size, in, out);
    }

    void emit(const aasm::GPReg out, const aasm::Address &in) override {
        m_as.mov(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::GPReg in) override {
        m_as.mov(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        if (out == in) {
            return;
        }
        unimplemented();
    }

    void emit(const aasm::GPReg out, const std::int64_t in) override {
        m_as.copy(m_size, in, out);
    }

    void emit(const aasm::Address &out, const std::int64_t in) override {
        m_as.mov(m_size, aasm::checked_cast<std::int32_t>(in), out);
    }

    std::uint8_t m_size;
    MasmEmitter& m_as;
};