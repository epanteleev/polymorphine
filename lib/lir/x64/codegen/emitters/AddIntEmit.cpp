#include "AddIntEmit.h"

void AddIntEmit::emit(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) {
    if (in1 == out) {
        m_as.add(m_size, in2, out);

    } else if (in2 == out) {
        m_as.add(m_size, in1, out);

    } else {
        m_as.copy(m_size, in1, out);
        m_as.add(m_size, in2, out);
    }
}

void AddIntEmit::emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) {
}

void AddIntEmit::emit(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) {
}

void AddIntEmit::emit(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) {
}

void AddIntEmit::emit(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) {
}

void AddIntEmit::emit(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) {
}

void AddIntEmit::emit(aasm::GPReg out, std::int32_t in1, std::int32_t in2) {
}

void AddIntEmit::emit(aasm::GPReg out, std::int32_t in1, const aasm::Address &in2) {
}

void AddIntEmit::emit(aasm::GPReg out, const aasm::Address &in1, std::int32_t in2) {
}

void AddIntEmit::emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) {
}

void AddIntEmit::emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) {
}

void AddIntEmit::emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) {
}

void AddIntEmit::emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) {
}

void AddIntEmit::emit(const aasm::Address &out, aasm::GPReg in1, std::int32_t in2) {
}

void AddIntEmit::emit(const aasm::Address &out, std::int32_t in1, std::int32_t in2) {
}

void AddIntEmit::emit(const aasm::Address &out, const aasm::Address &in1, std::int32_t in2) {
}

void AddIntEmit::emit(const aasm::Address &out, std::int32_t in1, const aasm::Address &in2) {
}

void AddIntEmit::emit(const aasm::Address &out, std::int32_t in1, aasm::GPReg in2) {
}
