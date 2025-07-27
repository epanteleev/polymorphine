#include "LoadGPEmit.h"

#include "lir/x64/asm/AsmEmitter.h"

void LoadGPEmit::emit(const aasm::GPReg out, const aasm::GPReg in) {
    m_as.mov(m_size, aasm::Address(in), out);
}

void LoadGPEmit::emit(aasm::GPReg out, const aasm::Address &in) {
    unimplemented();
}

void LoadGPEmit::emit(const aasm::Address &out, aasm::GPReg in) {
    unimplemented();
}

void LoadGPEmit::emit(const aasm::Address &out, const aasm::Address &in) {
    unimplemented();
}

void LoadGPEmit::emit(aasm::GPReg out, std::int64_t in) {
    unimplemented();
}

void LoadGPEmit::emit(const aasm::Address &out, std::int64_t in) {
    unimplemented();
}
