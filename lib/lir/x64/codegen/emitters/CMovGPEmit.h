#pragma once

#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/visitors/GPBinaryVisitor.h"


class CMovGPEmit final: public GPBinaryVisitor {
public:
    static void emit(MasmEmitter& as, const std::uint8_t size, const aasm::CondType cond_type, const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        CMovGPEmit emitter(as, cond_type, convert_byte_size(size));
        dispatch(emitter, out, in1, in2);
    }

private:
    friend class GPBinaryVisitor;

    explicit CMovGPEmit(MasmEmitter& as, aasm::CondType cond_type, const std::uint8_t size) noexcept:
        m_size(size),
        m_cond_type(cond_type),
        m_as(as) {}

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

    /**
     * x64 cmov instruction supports operands with size 2,4 and 8 bytes only.
     * The function converts byte to dword size.
     *
     * Word size also good choice, but the cmov instruction will have bigger size then dword.
     */
    static std::uint8_t convert_byte_size(const std::uint8_t size) {
        return size == 1 ? 4 : size;
    }

    std::uint8_t m_size;
    aasm::CondType m_cond_type;
    MasmEmitter& m_as;
};