#pragma once

template<typename TemporalRegStorage, typename AsmEmit>
class MovsxGPEmit final: public GPUnaryOutVisitor {
public:
    explicit MovsxGPEmit(const TemporalRegStorage& temporal_regs, AsmEmit &as, const std::uint8_t to_size, const std::uint8_t from_size) noexcept:
        m_to_size(to_size),
        m_from_size(from_size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const GPVReg& out, const GPOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryOutVisitor;

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.movsx(m_from_size, m_to_size, in, out);
    }

    void emit(const aasm::GPReg out, const aasm::Address &in) override {
        m_as.movsx(m_from_size, m_to_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::GPReg in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::GPReg out, const std::int64_t in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const std::int64_t in) override {
        unimplemented();
    }

    std::uint8_t m_to_size;
    std::uint8_t m_from_size;
    AsmEmit& m_as;
    const TemporalRegStorage& m_temporal_regs;
};