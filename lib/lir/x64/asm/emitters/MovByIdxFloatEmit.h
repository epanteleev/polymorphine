#pragma once

template<typename TempRegStorage, typename AsmEmit>
class MovByIdxFloatEmit final: public XBinaryVisitorWithGp {
public:
    explicit MovByIdxFloatEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(m_temporal_regs) {}

    void apply(const GPVReg &out, const GPOp &index, const XOp &value) {
        dispatch(*this, out, index, value);
    }

private:
    friend class XBinaryVisitorWithGp;

    void emit(aasm::GPReg out, aasm::GPReg in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::GPReg in1, const aasm::XmmReg in2) override {
        m_as.mov(cst::POINTER_SIZE, out, m_temporal_regs.gp_temp1());
        m_as.movfp(m_size, in2, aasm::Address(m_temporal_regs.gp_temp1(), in1, m_size, 0));
    }

    void emit(const aasm::Address &out, const aasm::GPReg in1, const aasm::Address &in2) override {
        m_as.mov(cst::POINTER_SIZE, out, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, in2, m_temporal_regs.gp_temp2());
        m_as.mov(m_size, m_temporal_regs.gp_temp2(), aasm::Address(m_temporal_regs.gp_temp1(), in1, m_size, 0));
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};