#pragma once

#include "lir/x64/analysis/regalloc/ClobberRegs.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/visitors/GPBinaryVisitor.h"


class CMovGPEmit final: public GPBinaryVisitor {
public:
    static void emit(MasmEmitter& as, const ClobberRegs& clobber_regs, const std::uint8_t size, const aasm::CondType cond_type, const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        CMovGPEmit emitter(clobber_regs, as, cond_type, convert_byte_size(size));
        dispatch(emitter, out, in1, in2);
    }

private:
    friend class GPBinaryVisitor;

    explicit CMovGPEmit(const ClobberRegs& regs, MasmEmitter& as, const aasm::CondType cond_type, const std::uint8_t size) noexcept:
        m_size(size),
        m_cond_type(cond_type),
        m_as(as),
        m_clobber_regs(regs) {}

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) override {
        if (in1 == in2) {
            m_as.copy(m_size, in1, out);
            return;
        }
        if (out == in1) {
            m_as.cmov(m_size, aasm::invert(m_cond_type), in2, out);
            return;
        }
        if (out == in2) {
            m_as.cmov(m_size, m_cond_type, in1, out);
            return;
        }

        m_as.copy(m_size, in2, out);
        m_as.cmov(m_size, m_cond_type, in1, out);
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override {
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

    void emit(const aasm::GPReg out, const std::int32_t in1, const std::int32_t in2) override {
        if (in1 == in2) {
            m_as.copy(m_size, in1, out);
            return;
        }

        m_as.mov(m_size, in2, out);
        m_as.mov(m_size, in1, m_clobber_regs.gp_temp1());
        m_as.cmov(m_size, m_cond_type, m_clobber_regs.gp_temp1(), out);
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

    void emit(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) override  {
        unimplemented();
    }

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
    const ClobberRegs& m_clobber_regs;
};