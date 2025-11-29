#pragma once

template<typename TempRegStorage, typename AsmEmit>
class StoreXmmEmit final: public XUnaryGpOutVisitor {
public:
    explicit StoreXmmEmit(const TempRegStorage& temporal_regs, AsmEmit &as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const GPVReg& out, const XOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class XUnaryGpOutVisitor;

    void emit(const aasm::GPReg out, aasm::XmmReg in) override {
        m_as.movfp(m_size, in, aasm::Address(out));
    }

    void emit(aasm::GPReg out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::XmmReg in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};