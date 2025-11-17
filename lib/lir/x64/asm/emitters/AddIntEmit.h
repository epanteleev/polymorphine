#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class AddIntEmit final: public GPBinaryVisitor {
public:
    explicit AddIntEmit(const TemporalRegStorage& reg_storage, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(reg_storage) {}

    void apply(const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        dispatch(*this, out, in1, in2);
    }

private:
    friend class GPBinaryVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) override {
        if (in1 == out) {
            m_as.add(m_size, in2, out);

        } else if (in2 == out) {
            m_as.add(m_size, in1, out);

        } else {
            m_as.copy(m_size, in1, out);
            m_as.add(m_size, in2, out);
        }
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::GPReg out, const aasm::Address &in1, const aasm::GPReg in2) override {
        if (in2 == out) {
            m_as.add(m_size, in1, out);

        } else {
            m_as.copy(m_size, in2, out);
            m_as.add(m_size, in1, out);
        }
    }

    void emit(const aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) override {
        m_as.mov(m_size, in1, out);
        m_as.add(m_size, in2, out);
    }

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const std::int64_t in2) override {
        assertion(std::in_range<std::int32_t>(in2), "Immediate value out of range for add instruction");
        if (in1 == out) {
            m_as.add(m_size, checked_cast<std::int32_t>(in2), out);
        } else {
            m_as.copy(m_size, in1, out);
            m_as.add(m_size, checked_cast<std::int32_t>(in2), out);
        }
    }

    void emit(aasm::GPReg out, std::int64_t in1, aasm::GPReg in2) override  {
        unimplemented();
    }
    void emit(aasm::GPReg out, std::int64_t in1, std::int64_t in2) override  {
        unimplemented();
    }
    void emit(aasm::GPReg out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::GPReg out, const aasm::Address &in1, const std::int64_t in2) override {
        assertion(std::in_range<std::int32_t>(in2), "Immediate value out of range for add instruction");
        m_as.mov(m_size, in1, out);
        m_as.add(m_size, static_cast<std::int32_t>(in2), out);
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