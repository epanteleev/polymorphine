#pragma once

template<typename TempRegStorage, typename AsmEmit>
class StoreOnStackGPEmit final: public GPBinaryAddrVisitor {
public:
    explicit StoreOnStackGPEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(m_temporal_regs) {}

    void apply(const aasm::Address& out, const GPOp& index, const GPOp& value) {
        dispatch(*this, out, index, value);
    }

private:
    friend class GPBinaryAddrVisitor;

    void emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const std::int64_t in1, aasm::GPReg in2) override {
        const auto offset = static_cast<std::int64_t>(m_size) * in1;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for store on stack");
        m_as.mov(m_size, in2, out.add_offset(offset));
    }

    void emit(const aasm::Address &out, const std::int64_t in1, const std::int64_t in2) override {
        const auto offset = static_cast<std::int64_t>(m_size) * in1;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for store on stack");

        if (std::in_range<std::int32_t>(in2)) {
            m_as.mov(m_size, static_cast<std::int32_t>(in2), out.add_offset(offset));

        } else {
            m_as.copy(m_size, in2, m_temporal_regs.gp_temp1());
            m_as.mov(m_size, m_temporal_regs.gp_temp1(), out.add_offset(offset));
        }
    }

    void emit(const aasm::Address &out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};