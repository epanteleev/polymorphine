#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class CopyFloatEmit final: public XUnaryOutVisitor {
public:
    explicit CopyFloatEmit(const TemporalRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const XVReg& out, const XOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class XUnaryOutVisitor;

    void emit(aasm::XmmReg out, aasm::XmmReg in) override {
        m_as.copyfp(m_size, in, out);
    }

    void emit(aasm::XmmReg out, const aasm::Address &in) override {
        m_as.movfp(m_size, in, out);
    }

    void emit(const aasm::Address &out, aasm::XmmReg in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::XmmReg out, const std::int64_t in) override {
        assertion(in == 0, "invariant");
        m_as.xorfp(m_size, out, out);
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};