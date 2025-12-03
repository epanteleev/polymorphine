#pragma once


template<typename TempRegStorage, typename AsmEmit>
class CvtUInt2FpEmit final: public GPUnaryXmmOutVisitor {
public:
    explicit CvtUInt2FpEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t from_size, const std::uint8_t to_size) noexcept:
            m_from_size(from_size),
            m_to_size(to_size),
            m_as(as),
            m_temporal_regs(m_temporal_regs) {}

    void apply(const XVReg &out, const GPOp &in) {
        dispatch(*this, out, in);
    }

private:
    friend class GPUnaryXmmOutVisitor;

    static constexpr auto TEMP_SIZE = 4;

    void emit(const aasm::XmmReg out, const aasm::GPReg in) override {
        switch (m_from_size) {
            case 1: [[fallthrough]];
            case 2: {
                const auto temp = m_temporal_regs.gp_temp1();
                m_as.movzx(m_from_size, TEMP_SIZE, in, temp);
                m_as.cvtsi2fp(TEMP_SIZE, m_to_size, temp, out);
                break;
            }
            case 4: {
                m_as.cvtsi2fp(cst::QWORD_SIZE, m_to_size, in, out);
                break;
            }
            case 8: {
                unimplemented();
            }
            default: die("unknown size {}", m_to_size);
        }
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

    std::uint8_t m_from_size{};
    std::uint8_t m_to_size{};
    AsmEmit& m_as;
    const TempRegStorage& m_temporal_regs;
};