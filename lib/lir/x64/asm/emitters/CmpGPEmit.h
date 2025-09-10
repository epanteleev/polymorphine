#pragma once

class CmpGPEmit final: public GPUnaryVisitor {
public:
    static void apply(MasmEmitter& as, const std::uint8_t size, const GPOp& out, const GPOp& in) {
        CmpGPEmit emitter(as, size);
        dispatch(emitter, out, in);
    }

private:
    friend class GPUnaryVisitor;

    explicit CmpGPEmit(MasmEmitter& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as) {}

    void emit(const aasm::GPReg out, const aasm::GPReg in) override {
        m_as.cmp(m_size, in, out);
    }

    void emit(aasm::GPReg out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in) override {
        unimplemented();
    }

    void emit(const aasm::GPReg out, const std::int64_t in) override {
        m_as.cmp(m_size, aasm::checked_cast<std::int32_t>(in), out);
    }

    void emit(const aasm::Address &out, std::int64_t in) override {
        unimplemented();
    }

    void emit(std::int64_t in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(std::int64_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    MasmEmitter& m_as;
};
