#include "CopyGPEmit.h"

void CopyGPEmit::rr(const aasm::GPReg out, const aasm::GPReg in) {
    m_as.copy(m_size, in, out);
}

void CopyGPEmit::rm(aasm::GPReg out, const aasm::Address &in) {
}

void CopyGPEmit::mr(const aasm::Address &out, aasm::GPReg in) {
}

void CopyGPEmit::mm(const aasm::Address &out, const aasm::Address &in) {
}

void CopyGPEmit::ri(aasm::GPReg out, std::int64_t in) {
}

void CopyGPEmit::mi(const aasm::Address &out, std::int64_t in) {
}
