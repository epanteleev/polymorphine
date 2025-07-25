#include "CmpGPEmit.h"

#include "lir/x64/asm/AsmEmitter.h"

void CmpGPEmit::emit(aasm::GPReg out, aasm::GPReg in) {
    m_as.cmp(m_size, in, out);
}

void CmpGPEmit::emit(aasm::GPReg out, const aasm::Address &in) {
}

void CmpGPEmit::emit(const aasm::Address &out, aasm::GPReg in) {
}

void CmpGPEmit::emit(const aasm::Address &out, const aasm::Address &in) {
}

void CmpGPEmit::emit(const aasm::GPReg out, const std::int64_t in) {
    assertion(std::in_range<std::int32_t>(in), "Immediate value for cmp must be in range of 32-bit signed integer");
    m_as.cmp(m_size, static_cast<std::int32_t>(in), out);
}

void CmpGPEmit::emit(const aasm::Address &out, std::int64_t in) {
}

void CmpGPEmit::emit(std::int64_t in1, std::int64_t in2) {
}

void CmpGPEmit::emit(std::int64_t in1, aasm::GPReg in2) {
}

void CmpGPEmit::emit(std::int64_t in1, const aasm::Address &in2) {
}
