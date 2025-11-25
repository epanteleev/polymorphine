#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class LoadByIdxFloatEmit final: public XBinaryVisitorXOut {
public:
    explicit LoadByIdxFloatEmit(const TemporalRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const XVReg& out, const GPOp& pointer, const GPOp& index) {
        dispatch(*this, out, pointer, index);
    }

private:
    friend class XBinaryVisitorXOut;

    void emit(aasm::XmmReg out, aasm::GPReg in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, const aasm::Address &in1, const aasm::GPReg in2) override {
        m_as.mov(cst::POINTER_SIZE, in1, m_temporal_regs.gp_temp1());
        m_as.movfp(m_size, aasm::Address(m_temporal_regs.gp_temp1(), in2, m_size, 0), out);
    }

    void emit(aasm::XmmReg out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, aasm::GPReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, std::int64_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, std::int64_t in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, const aasm::Address &in1, std::int64_t in2) override {
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

    void emit(const aasm::Address &out, std::int64_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};