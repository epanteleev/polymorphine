#pragma once

template<typename TempRegStorage, typename AsmEmit>
class StoreGPEmit final: public GPUnaryOutVisitor {
public:
    explicit StoreGPEmit(const TempRegStorage& temporal_regs, AsmEmit &as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const GPVReg& out, const GPOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.mov(m_size, in, aasm::Address(out));
    }

    void emit(const aasm::GPReg out, const aasm::Address &in) override {
        m_as.mov(m_size, in, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, m_temporal_regs.gp_temp1(), aasm::Address(out));
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
    const TempRegStorage& m_temporal_regs;
};