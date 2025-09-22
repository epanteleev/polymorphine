#pragma once

class SubIntEmit final: public GPBinaryVisitor {
public:
    static void apply(MasmEmitter& as, const std::uint8_t size, const GPVReg& out, const GPOp& in1, const GPOp& in2) {
        SubIntEmit emitter(as, size);
        dispatch(emitter, out, in1, in2);
    }

private:
    friend class GPBinaryVisitor;

    explicit SubIntEmit(MasmEmitter& as, const std::uint8_t size) noexcept:
        m_size(size),
        m_as(as) {}

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) override {
        if (out == in1) {
            m_as.sub(m_size, in2, out);
            return;
        }

        if (out == in2) {
            m_as.neg(m_size, out);
            m_as.add(m_size, in1, out);
            return;
        }

        m_as.copy(m_size, in1, out);
        m_as.sub(m_size, in2, out);
    }

    void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(const aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) override {
        m_as.mov(m_size, in1, out);
        m_as.sub(m_size, in2, out);
    }

    void emit(const aasm::GPReg out, const aasm::GPReg in1, const std::int64_t in2) override {
        assertion(std::in_range<std::int32_t>(in2), "Immediate value out of range for add instruction");
        if (out == in1) {
            m_as.sub(m_size, in2, out);
        } else {
            m_as.copy(m_size, in1, out);
            m_as.sub(m_size, in2, out);
        }
    }

    void emit(aasm::GPReg out, std::int64_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(aasm::GPReg out, const aasm::Address &in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, aasm::GPReg in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, const aasm::Address &in1, std::int64_t in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in1, const aasm::Address &in2) override {
        unimplemented();
    }

    void emit(const aasm::Address &out, std::int64_t in1, aasm::GPReg in2) override {
        unimplemented();
    }

    std::uint8_t m_size;
    MasmEmitter& m_as;
};