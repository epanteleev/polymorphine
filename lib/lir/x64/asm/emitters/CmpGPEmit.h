#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class CmpGPEmit final: public GPUnaryVisitor {
public:
    explicit CmpGPEmit(const TemporalRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const GPOp& out, const GPOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.cmp(m_size, in, out);
    }

    void emit(aasm::GPReg out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in) override {
        m_as.cmp(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        m_as.mov(m_size, in, m_temporal_regs.gp_temp1());
        m_as.cmp(m_size, m_temporal_regs.gp_temp1(), out);
    }

    void emit(const aasm::GPReg out, const std::int64_t in) override {
        m_as.cmp(m_size, checked_cast<std::int32_t>(in), out);
    }

    void emit(const aasm::Address &out, const std::int64_t in) override {
        m_as.cmp(m_size, checked_cast<std::int32_t>(in), out);
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
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};