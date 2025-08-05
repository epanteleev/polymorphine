//
// Created by epanteleev on 8/5/25.
//

#include "SubIntEmit.h"

void SubIntEmit::emit(aasm::GPReg out, aasm::GPReg in1, aasm::GPReg in2) {
    unimplemented();
}

void SubIntEmit::emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address &in2) {
    unimplemented();
}

void SubIntEmit::emit(aasm::GPReg out, const aasm::Address &in1, aasm::GPReg in2) {
    unimplemented();
}

void SubIntEmit::emit(aasm::GPReg out, const aasm::Address &in1, const aasm::Address &in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::GPReg out, const aasm::GPReg in1, const std::int32_t in2) {
    if (out == in1) {
        m_as.sub(m_size, in2, out);
    } else {
        m_as.copy(m_size, in1, out);
        m_as.sub(m_size, in2, out);
    }
}

void SubIntEmit::emit(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) {
    unimplemented();
}

void SubIntEmit::emit(aasm::GPReg out, std::int32_t in1, std::int32_t in2) {
    unimplemented();
}

void SubIntEmit::emit(aasm::GPReg out, std::int32_t in1, const aasm::Address &in2) {
    unimplemented();
}

void SubIntEmit::emit(aasm::GPReg out, const aasm::Address &in1, std::int32_t in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, aasm::GPReg in1, aasm::GPReg in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, aasm::GPReg in1, const aasm::Address &in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, const aasm::Address &in1, aasm::GPReg in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, const aasm::Address &in1, const aasm::Address &in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, aasm::GPReg in1, std::int32_t in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, std::int32_t in1, std::int32_t in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, const aasm::Address &in1, std::int32_t in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, std::int32_t in1, const aasm::Address &in2) {
    unimplemented();
}

void SubIntEmit::emit(const aasm::Address &out, std::int32_t in1, aasm::GPReg in2) {
    unimplemented();
}