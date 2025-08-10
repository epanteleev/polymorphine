#include "CMovGPEmit.h"

void CMovGPEmit::emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) {
    if (in1 == in2) {
        m_as.copy(m_size, in1, out);
        return;
    }
    if (out == in1) {
        m_as.cmov(m_size, aasm::invert(m_cond_type), in2, out);
        return;
    }
    if (out == in2) {
        m_as.cmov(m_size, m_cond_type, in1, out);
        return;
    }

    m_as.copy(m_size, in2, out);
    m_as.cmov(m_size, m_cond_type, in1, out);
}

void CMovGPEmit::emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) {
    unimplemented();
}

void CMovGPEmit::emit(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) {
    unimplemented();
}

void CMovGPEmit::emit(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) {
    unimplemented();
}

void CMovGPEmit::emit(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) {
    unimplemented();
}

void CMovGPEmit::emit(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) {
    unimplemented();
}

void CMovGPEmit::emit(aasm::GPReg out, std::int32_t in1, std::int32_t in2) {
    unimplemented();
}

void CMovGPEmit::emit(aasm::GPReg out, std::int32_t in1, const aasm::Address &in2) {
    unimplemented();
}

void CMovGPEmit::emit(aasm::GPReg out, const aasm::Address &in1, std::int32_t in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, aasm::GPReg in1, std::int32_t in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, std::int32_t in1, std::int32_t in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, const aasm::Address &in1, std::int32_t in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, std::int32_t in1, const aasm::Address &in2) {
    unimplemented();
}

void CMovGPEmit::emit(const aasm::Address &out, std::int32_t in1, aasm::GPReg in2) {
    unimplemented();
}