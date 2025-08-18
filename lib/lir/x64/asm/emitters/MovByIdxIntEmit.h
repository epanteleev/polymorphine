#pragma once

#include "lir/x64/asm/visitors/GPBinaryVisitor.h"

class MovByIdxIntEmit final: public GPBinaryVisitor {
public:
    explicit MovByIdxIntEmit(MasmEmitter& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as) {}

    void emit(const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        dispatch(*this, out, in1, in2);
    }

private:
    friend class GPBinaryVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) override {
        m_as.mov(m_size, in2, aasm::Address(out, in1, m_size, 0));
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int32_t in1, std::int32_t in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int32_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, std::int32_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, std::int32_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int32_t in1, std::int32_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, std::int32_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int32_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int32_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    MasmEmitter& m_as;
};