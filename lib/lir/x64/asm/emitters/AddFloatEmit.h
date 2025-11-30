#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class AddFloatEmit final: public XBinaryVisitor {
public:
    explicit AddFloatEmit(const TemporalRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const XVReg& out, const XOp& in1, const XOp& in2) {
        dispatch(*this, out, in1, in2);
    }

private:
    friend class XBinaryVisitor;

    void emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override {
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

    void emit(aasm::XmmReg out, aasm::XmmReg in1, aasm::XmmReg in2) override {
        if (in1 == out) {
            m_as.addfp(m_size, in2, out);

        } else if (in2 == out) {
            m_as.addfp(m_size, in1, out);

        } else {
            m_as.copyfp(m_size, in1, out);
            m_as.addfp(m_size, in2, out);
        }
    }

    void emit(aasm::XmmReg out, aasm::XmmReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::XmmReg out, const aasm::Address &in1, const aasm::XmmReg in2) override {
        if (out == in2) {
            m_as.addfp(m_size, in1, out);

        } else {
            m_as.movfp(m_size, in1, out);
            m_as.addfp(m_size, in2, out);
        }
    }

    void emit(aasm::XmmReg out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, aasm::XmmReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, std::int64_t in1, aasm::XmmReg in2) override {
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

    void emit(const aasm::Address &out, aasm::XmmReg in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::XmmReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::XmmReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};