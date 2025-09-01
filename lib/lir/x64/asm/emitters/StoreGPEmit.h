#pragma once

#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"

class StoreGPEmit final: public GPUnaryOutVisitor {
public:
    static void apply(MasmEmitter &as, const std::uint8_t size, const GPVReg& out, const GPOp& in) {
        StoreGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    explicit StoreGPEmit(MasmEmitter &as, const std::uint8_t size) noexcept
        : m_size(size),
          m_as(as) {}

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.mov(m_size, in, aasm::Address(out));
    }

    void emit(aasm::GPReg out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    std::uint8_t m_size;
    MasmEmitter& m_as;
};

