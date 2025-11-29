#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class LoadFloatEmit final: public GPUnaryXmmOutVisitor {
public:
    explicit LoadFloatEmit(const TemporalRegStorage& reg_storage, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(reg_storage) {}

    void apply(const XVReg& out, const GPOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryXmmOutVisitor;

    void emit(const aasm::XmmReg out, const aasm::GPReg in) override {
        m_as.movfp(m_size, aasm::Address(in), out);
    }

    void emit(aasm::XmmReg out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in) override {
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

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};