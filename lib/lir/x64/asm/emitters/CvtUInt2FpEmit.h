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

    void cvt_u64_to_fp(const aasm::XmmReg out, const aasm::GPReg in) {
        const auto slow_path = m_as.create_label();
        const auto cont = m_as.create_label();

        m_as.test(cst::QWORD_SIZE, in, in);
        m_as.jcc(aasm::CondType::S, slow_path);

        // Fast Path
        m_as.cvtsi2fp(cst::QWORD_SIZE, m_to_size, in, out);
        m_as.jmp(cont);

        m_as.set_label(slow_path);
        // Slow Path
        const auto temp1 = m_temporal_regs.gp_temp1();
        const auto temp2 = m_temporal_regs.gp_temp2();
        m_as.copy(m_from_size, in, temp1);
        m_as.copy(m_from_size, in, temp2);
        m_as.shift(cst::QWORD_SIZE, ShiftKind::SHR, 1, temp1);
        m_as.aand(m_from_size, 1, temp2);
        m_as.oor(cst::QWORD_SIZE, temp1, temp2);
        m_as.xorfp(m_to_size, out, out);
        m_as.cvtsi2fp(cst::QWORD_SIZE, m_to_size, temp2, out);
        m_as.addfp(m_to_size, out, out);

        m_as.set_label(cont);
    }

    void emit(const aasm::XmmReg out, const aasm::GPReg in) override {
        switch (m_from_size) {
            case 1: [[fallthrough]];
            case 2: {
                const auto temp = m_temporal_regs.gp_temp1();
                m_as.movzx(m_from_size, TEMP_SIZE, in, temp);
                m_as.cvtsi2fp(TEMP_SIZE, m_to_size, temp, out);
                break;
            }
            case 4: m_as.cvtsi2fp(cst::QWORD_SIZE, m_to_size, in, out); break;
            case 8: cvt_u64_to_fp(out, in); break;
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