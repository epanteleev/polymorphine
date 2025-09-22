#pragma once

template<typename TempRegStorage, typename AsmEmit>
class MovByIdxIntEmit final: public GPBinaryVisitor {
public:
    explicit MovByIdxIntEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(m_temporal_regs) {}

    void apply(const GPVReg& out, const GPOp& index, const GPOp& value) {
        dispatch(*this, out, index, value);
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

    void emit(aasm::GPReg out, aasm::GPReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::GPReg out, const std::int64_t in1, const aasm::GPReg in2) override {
        const auto offset = static_cast<std::int64_t>(m_size) * in1;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for mov by idx");
        m_as.mov(m_size, in2, aasm::Address(out, offset));
    }

    void emit(const aasm::GPReg out, const std::int64_t in1, const std::int64_t in2) override {
        const auto offset = static_cast<std::int64_t>(m_size) * in1;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for mov by idx");

        if (std::in_range<std::int32_t>(in2)) {
            m_as.mov(m_size, static_cast<std::int32_t>(in2), aasm::Address(out, offset));

        } else {
            m_as.copy(m_size, in2, m_temporal_regs.gp_temp1());
            m_as.mov(m_size, m_temporal_regs.gp_temp1(), aasm::Address(out, offset));
        }
    }

    void emit(aasm::GPReg out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) override {
        m_as.mov(8, out, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, in2, aasm::Address(m_temporal_regs.gp_temp1(), in1, m_size, 0));
    }

    void emit(const aasm::Address &out, const aasm::GPReg in1, const aasm::Address &in2) override {
        m_as.mov(8, out, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, in2, m_temporal_regs.gp_temp2());
        m_as.mov(m_size, m_temporal_regs.gp_temp2(), aasm::Address(m_temporal_regs.gp_temp1(), in1, m_size, 0));
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) override {
        m_as.mov(8, out, m_temporal_regs.gp_temp1());
        m_as.mov(8, in1, m_temporal_regs.gp_temp2());
        m_as.mov(m_size, in2, aasm::Address(m_temporal_regs.gp_temp1(), m_temporal_regs.gp_temp2(), m_size, 0));
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