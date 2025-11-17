#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class CopyGPEmit final: public GPUnaryOutVisitor {
public:
    explicit CopyGPEmit(const TemporalRegStorage& reg_storage, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(reg_storage) {}

    void apply(const GPVReg& out, const GPOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in) override  {
        m_as.copy(m_size, in, out);
    }

    void emit(const aasm::GPReg out, const aasm::Address &in) override {
        m_as.mov(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::GPReg in) override {
        m_as.mov(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        if (out == in) {
            return;
        }
        unimplemented();
    }

    void emit(const aasm::GPReg out, const std::int64_t in) override {
        m_as.copy(m_size, in, out);
    }

    void emit(const aasm::Address &out, const std::int64_t in) override {
        m_as.mov(m_size, checked_cast<std::int32_t>(in), out);
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};