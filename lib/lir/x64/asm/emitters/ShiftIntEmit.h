#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class ShiftIntEmit final: public GPBinaryVisitor {
public:
    explicit ShiftIntEmit(const TemporalRegStorage& reg_storage, AsmEmit& as, const ShiftKind kind, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(reg_storage),
        m_kind(kind) {}

    void apply(const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        dispatch(*this, out, in1, in2);
    }

private:
    friend class GPBinaryVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) override {
        assertion(in2 == aasm::rcx, "must be");
        if (out == in1) {
            m_as.shift(m_size, m_kind, out);

        } else if (out == in2) {
            const auto temp = m_temporal_regs.gp_temp1();
            m_as.copy(m_size, in1, temp);
            m_as.shift(m_size, m_kind, temp);
            m_as.copy(m_size, temp, out);

        } else {
            m_as.copy(m_size, in1, out);
            m_as.shift(m_size, m_kind, out);
        }
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
        if (out == in1) {
            m_as.shift(m_size, m_kind, in2, out);
        } else {
            m_as.copy(m_size, in1, out);
            m_as.shift(m_size, m_kind, in2, out);
        }
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
    ShiftKind m_kind;
};
