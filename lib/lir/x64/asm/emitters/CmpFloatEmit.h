#pragma once


template<typename TemporalRegStorage, typename AsmEmit>
class CmpFloatEmit final: public XUnaryVisitor {
public:
    explicit CmpFloatEmit(const TemporalRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const XOp& out, const XOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class XUnaryVisitor;

    void emit(aasm::XmmReg out, aasm::XmmReg in) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, const aasm::Address &in) override {
        m_as.ucmpfp(m_size, in, out);
    }

    void emit(const aasm::Address &out, aasm::XmmReg in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, std::int64_t in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    void emit(std::int64_t out, std::int64_t in) override {
        unimplemented();
    }

    void emit(std::int64_t out, const aasm::Address& in) override {
        unimplemented();
    }

    void emit(std::int64_t out, const aasm::XmmReg& in) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};