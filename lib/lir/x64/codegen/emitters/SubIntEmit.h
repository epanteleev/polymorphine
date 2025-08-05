#pragma once

#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/visitors/GPBinaryVisitor.h"

class SubIntEmit final: public GPBinaryVisitor {
public:
    static void emit(MasmEmitter& as, const std::uint8_t size, const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        SubIntEmit emitter(as, size);
        dispatch(emitter, out, in1, in2);
    }

private:
    friend class GPBinaryVisitor;

    explicit SubIntEmit(MasmEmitter& as, const std::uint8_t size) noexcept
        : m_size(size), m_as(as) {}

    void emit(aasm::GPReg out, aasm::GPReg in1, aasm::GPReg in2) override;
    void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override;
    void emit(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) override;
    void emit(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) override;
    void emit(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) override;
    void emit(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) override;
    void emit(aasm::GPReg out, std::int32_t in1, std::int32_t in2) override;
    void emit(aasm::GPReg out, std::int32_t in1, const aasm::Address &in2) override;
    void emit(aasm::GPReg out, const aasm::Address &in1, std::int32_t in2) override;
    void emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) override;
    void emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) override;
    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) override;
    void emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override;
    void emit(const aasm::Address &out, aasm::GPReg in1, std::int32_t in2) override;
    void emit(const aasm::Address &out, std::int32_t in1, std::int32_t in2) override;
    void emit(const aasm::Address &out, const aasm::Address &in1, std::int32_t in2) override;
    void emit(const aasm::Address &out, std::int32_t in1, const aasm::Address &in2) override;
    void emit(const aasm::Address &out, std::int32_t in1, aasm::GPReg in2) override;

    std::uint8_t m_size;
    MasmEmitter& m_as;
};
