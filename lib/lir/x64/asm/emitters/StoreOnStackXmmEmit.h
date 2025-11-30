#pragma once

template<typename TempRegStorage, typename AsmEmit>
class StoreOnStackXmmEmit final: public XBinaryAddrVisitor {
public:
    explicit StoreOnStackXmmEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(m_temporal_regs) {}

    void apply(const aasm::Address& out, const GPOp& index, const XOp& value) {
        dispatch(*this, out, index, value);
    }

private:
    friend class XBinaryAddrVisitor;

    void emit(const aasm::Address &out, aasm::GPReg in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const std::int64_t in1, const std::int64_t in2) override {
        const auto offset = static_cast<std::int64_t>(m_size) * in1;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for store on stack");
        assertion(in2 == 0, "invariant");

        const auto temp = m_temporal_regs.xmm_temp1();
        m_as.xorfp(m_size, temp, temp);
        m_as.movfp(m_size, temp, out.add_offset(offset));
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const std::int64_t in1, const aasm::Address &in2) override {
        const auto offset = static_cast<std::int64_t>(m_size) * in1;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for store on stack");
        m_as.mov(m_size, in2, m_temporal_regs.gp_temp1());
        m_as.mov(m_size, m_temporal_regs.gp_temp1(), out.add_offset(offset));
    }

    void emit(const aasm::Address &out, std::int64_t in1, aasm::XmmReg in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};