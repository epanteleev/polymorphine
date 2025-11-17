#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class LoadGPEmit final: public GPUnaryOutVisitor {
public:
    explicit LoadGPEmit(const TemporalRegStorage& reg_storage, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(reg_storage) {}

    void apply(const GPVReg& out, const GPOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.mov(m_size, aasm::Address(in), out);
    }

    void emit(aasm::GPReg out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};

