#pragma once

#include "lir/x64/lir_frwd.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"


class LoadGPEmit final: public GPUnaryOutVisitor {
public:
    static void emit(MasmEmitter& as, const std::uint8_t size, const GPVReg& out, const GPOp& in) {
        LoadGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    explicit LoadGPEmit(MasmEmitter& as, const std::uint8_t size) noexcept
        : m_size(size), m_as(as) {}

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.mov(m_size, aasm::Address(in), out);
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

