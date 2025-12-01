#pragma once

template<typename TempRegStorage, typename AsmEmit>
class CvtFp2IntEmit final: public XUnaryGpOutVisitor {
public:
    explicit CvtFp2IntEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t from_size, const std::uint8_t to_size) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_as(as),
            m_temporal_regs(m_temporal_regs) {}

    void apply(const GPVReg &out, const XOp &in) {
        dispatch(*this, out, in);
    }

private:
    friend class XUnaryGpOutVisitor;

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, aasm::XmmReg in) override {
        const auto actual_size = m_to_size >= 4 ? m_to_size : 4;
        m_as.cvtfp2si(m_from_size, actual_size, in, out);
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

    std::uint8_t m_from_size;
    std::uint8_t m_to_size;
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};
