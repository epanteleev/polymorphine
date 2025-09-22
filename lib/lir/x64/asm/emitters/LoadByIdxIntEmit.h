#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class LoadByIdxIntEmit final: public GPBinaryVisitor {
public:
    explicit LoadByIdxIntEmit(const TemporalRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const GPVReg& out, const GPOp& pointer, const GPOp& index) {
        dispatch(*this, out, pointer, index);
    }

private:
    friend class GPBinaryVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) override {
        m_as.mov(m_size, aasm::Address(in1, in2, m_size, 0), out);
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::GPReg out, const aasm::Address &in1, const aasm::GPReg in2) override {
        m_as.mov(8, in1, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, aasm::Address(m_temporal_regs.gp_temp1(), in2, m_size, 0), out);
    }

    void emit(const aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) override {
        m_as.mov(8, in2, m_temporal_regs.gp_temp1());
        m_as.mov(8, in1, m_temporal_regs.gp_temp2());
        m_as.mov(m_size, aasm::Address(m_temporal_regs.gp_temp2(), m_temporal_regs.gp_temp1(), m_size, 0), out);
    }

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const std::int64_t in2) override {
        const auto offset = m_size * in2;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range");
        m_as.mov(m_size, aasm::Address(in1, static_cast<std::int32_t>(offset)), out);
    }

    void emit(aasm::GPReg out, std::int64_t in1, aasm::GPReg in2) override {
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
