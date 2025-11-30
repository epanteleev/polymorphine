#pragma once

template<typename TempRegStorage, typename AsmEmit>
class LoadFromStackFloatEmit final: public XBinarySrcAddrVisitor {
public:
    explicit LoadFromStackFloatEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as),
        m_temporal_regs(m_temporal_regs) {}

    void apply(const XVReg& inout, const GPOp& index, const aasm::Address& value) {
        dispatch(*this, inout, index, value);
    }

private:
    friend class XBinarySrcAddrVisitor;

    void emit(aasm::XmmReg out, aasm::GPReg in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, const aasm::Address &in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(aasm::XmmReg out, const std::int64_t in, const aasm::Address &src) override {
        const auto offset = static_cast<std::int64_t>(m_size) * in;
        assertion(std::in_range<std::int32_t>(offset), "Offset out of range for load stack address");

        m_as.movfp(m_size, src.add_offset(offset), out);
    }

    void emit(const aasm::Address &out, const aasm::Address &in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in, const aasm::Address &src) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in, const aasm::Address &src) override {
        unimplemented();
    }

    std::uint8_t m_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};