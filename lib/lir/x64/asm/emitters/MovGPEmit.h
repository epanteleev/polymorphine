#pragma once

#include "lir/x64/asm/visitors/GPUnaryAddrVisitor.h"

class MovGPEmit final: public GPUnaryAddrVisitor {
public:
    static void emit(MasmEmitter& as, const std::uint8_t size, const aasm::Address& out, const GPOp& in) {
        MovGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryAddrVisitor;

    explicit MovGPEmit(MasmEmitter& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as) {}

    void emit(const aasm::Address &out, const aasm::GPReg in) override {
        m_as.mov(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const std::int64_t in) override {
        m_as.mov(m_size, aasm::checked_cast<std::int32_t>(in), out);
    }

    std::uint8_t m_size;
    MasmEmitter& m_as;
};
