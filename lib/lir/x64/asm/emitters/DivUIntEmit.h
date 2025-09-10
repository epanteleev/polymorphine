#pragma once

template<typename TempRegStorage, typename AsmEmit>
class DivUIntEmit final: public GPBinaryVisitor {
public:
    explicit DivUIntEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(m_temporal_regs) {}

    void apply(const GPVReg& out, const GPOp& dividend, const GPOp& divider) {
        dispatch(*this, out, dividend, divider);
    }

private:
    friend class GPBinaryVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) override {
        m_as.copy(m_size, in1, aasm::rax);
        m_as.xxor(m_size, aasm::rdx, aasm::rdx);
        m_as.div(m_size, in2);
        m_as.copy(m_size, aasm::rax, out);
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

    void emit(aasm::GPReg out, aasm::GPReg in1, std::int64_t in2) override {
        m_as.copy(m_size, in1, aasm::rax);
        m_as.copy(m_size, in2, m_temporal_regs.gp_temp1());
        m_as.xxor(m_size, aasm::rdx, aasm::rdx);
        m_as.div(m_size, m_temporal_regs.gp_temp1());
        m_as.copy(m_size, aasm::rax, out);
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
    const TempRegStorage& m_temporal_regs;
};
