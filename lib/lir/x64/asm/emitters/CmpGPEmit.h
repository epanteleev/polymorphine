#pragma once

#include "asm/asm.h"
#include "lir/x64/lir_frwd.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"

class CmpGPEmit final: public GPUnaryVisitor {
public:
    static void emit(MasmEmitter& as, const std::uint8_t size, const GPOp& out, const GPOp& in) {
        CmpGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryVisitor;

    explicit CmpGPEmit(MasmEmitter& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as) {}

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.cmp(m_size, in, out);
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

    void emit(const aasm::GPReg out, const std::int64_t in) override {
        assertion(std::in_range<std::int32_t>(in), "Immediate value for cmp must be in range of 32-bit signed integer");
        m_as.cmp(m_size, static_cast<std::int32_t>(in), out);
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    void emit(std::int64_t in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(std::int64_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    MasmEmitter& m_as;
};
