#pragma once

template<typename TempRegStorage, typename AsmEmit>
class MovGPEmit final: public GPUnaryAddrVisitor {
public:
    explicit MovGPEmit(const TempRegStorage& temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(temporal_regs) {}

    void apply(const aasm::Address& out, const GPOp& in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryAddrVisitor;

    void emit(const aasm::Address &out, const aasm::GPReg in) override {
        m_as.mov(m_size, in, out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        if (out == in) {
            return;
        }

        m_as.mov(m_size, in, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, m_temporal_regs.gp_temp1(), out);
    }

    void emit(const aasm::Address &out, const std::int64_t in) override {
        m_as.mov(m_size, aasm::checked_cast<std::int32_t>(in), out);
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};
