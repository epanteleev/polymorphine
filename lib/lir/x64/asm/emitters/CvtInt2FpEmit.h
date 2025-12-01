#pragma once

template<typename TempRegStorage, typename AsmEmit>
class CvtInt2FpEmit final: public GPUnaryXmmOutVisitor {
public:
    explicit CvtInt2FpEmit(const TempRegStorage& m_temporal_regs, AsmEmit& as, const std::uint8_t from_size, const std::uint8_t to_size) noexcept:
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

    [[nodiscard]]
    std::optional<aasm::GPReg> try_convert(const aasm::GPReg src) {
        if (m_from_size == 4 || m_from_size == 8) {
            return std::nullopt;
        }

        const auto temp = m_temporal_regs.gp_temp1();
        m_as.movsx(m_from_size, TEMP_SIZE, src, temp);
        return temp;
    }

    void emit(const aasm::XmmReg out, const aasm::GPReg in) override {
        if (const auto temp = try_convert(in); temp.has_value()) {
            m_as.cvtsi2fp(TEMP_SIZE, m_to_size, temp.value(), out);

        } else {
            m_as.cvtsi2fp(m_from_size, m_to_size, in, out);
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