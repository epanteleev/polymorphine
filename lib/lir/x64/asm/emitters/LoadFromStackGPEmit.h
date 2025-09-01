#pragma once
#include "lir/x64/asm/visitors/GPBinarySrcAddrVisitor.h"
#include "lir/x64/codegen/LIRFunctionCodegen.h"

template<typename TempRegStorage, typename AsmEmit>
class LoadFromStackGPEmit final: public GPBinarySrcAddrVisitor {
public:
    explicit LoadFromStackGPEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(m_temporal_regs) {}

    void apply(const GPVReg& inout, const GPOp& index, const aasm::Address& value) {
        dispatch(*this, inout, index, value);
    }

private:
    friend class GPBinarySrcAddrVisitor;

    void emit(aasm::GPReg out, aasm::GPReg in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const std::int64_t in, const aasm::Address &src) override {
        const auto offset = static_cast<std::int64_t>(src.offset()) + m_size * in;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for load from stack");

        m_as.mov(m_size, aasm::Address(src.base().value(), offset), out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in, const aasm::Address &src) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};
