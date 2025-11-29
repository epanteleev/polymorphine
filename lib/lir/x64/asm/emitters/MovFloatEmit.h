#pragma once

template<typename TempRegStorage, typename AsmEmit>
class MovFloatEmit final: public XUnaryAddrVisitor {
public:
    explicit MovFloatEmit(const TempRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const aasm::Address& out, const XOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class XUnaryAddrVisitor;

    void emit(const aasm::Address &out, const aasm::XmmReg in) override {
        m_as.movfp(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        if (out == in) {
            return;
        }

        m_as.mov(m_size, in, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, m_temporal_regs.gp_temp1(), out);
    }

    void emit(const aasm::Address &out, const std::int64_t in) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};