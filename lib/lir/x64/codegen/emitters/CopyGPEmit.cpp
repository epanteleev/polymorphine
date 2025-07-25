#include "CopyGPEmit.h"

#include "lir/x64/asm/AsmEmitter.h"

void CopyGPEmit::emit(const aasm::GPReg out, const aasm::GPReg in) {
    m_as.copy(m_size, in, out);
}

void CopyGPEmit::emit(aasm::GPReg out, const aasm::Address &in) {
}

void CopyGPEmit::emit(const aasm::Address &out, aasm::GPReg in) {
}

void CopyGPEmit::emit(const aasm::Address &out, const aasm::Address &in) {
}

void CopyGPEmit::emit(const aasm::GPReg out, const std::int64_t in) {
    m_as.copy(m_size, in, out);
}

void CopyGPEmit::emit(const aasm::Address &out, std::int64_t in) {
}
