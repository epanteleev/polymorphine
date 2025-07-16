#include "AddIntEmit.h"

void AddIntEmit::rrr(const aasm::GPReg out, const aasm::GPReg in1, const aasm::GPReg in2) {
    if (in1 == out) {
        m_as.add(m_size, in2, out);

    } else if (in2 == out) {
        m_as.add(m_size, in1, out);

    } else {
        m_as.copy(m_size, in1, out);
        m_as.add(m_size, in2, out);
    }
}

void AddIntEmit::rrm(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) {
}

void AddIntEmit::rmr(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) {
}

void AddIntEmit::rmm(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) {
}

void AddIntEmit::rri(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) {
}

void AddIntEmit::rir(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) {
}

void AddIntEmit::rii(aasm::GPReg out, std::int32_t in1, std::int32_t in2) {
}

void AddIntEmit::rim(aasm::GPReg out, std::int32_t in1, const aasm::Address &in2) {
}

void AddIntEmit::rmi(aasm::GPReg out, const aasm::Address &in1, std::int32_t in2) {
}

void AddIntEmit::mrr(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) {
}

void AddIntEmit::mrm(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) {
}

void AddIntEmit::mmr(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) {
}

void AddIntEmit::mmm(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) {
}

void AddIntEmit::mri(const aasm::Address &out, aasm::GPReg in1, std::int32_t in2) {
}

void AddIntEmit::mii(const aasm::Address &out, std::int32_t in1, std::int32_t in2) {
}

void AddIntEmit::mmi(const aasm::Address &out, const aasm::Address &in1, std::int32_t in2) {
}

void AddIntEmit::mim(const aasm::Address &out, std::int32_t in1, const aasm::Address &in2) {
}

void AddIntEmit::mir(const aasm::Address &out, std::int32_t in1, aasm::GPReg in2) {
}
