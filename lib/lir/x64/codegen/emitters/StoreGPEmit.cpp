#include "StoreGPEmit.h"

#include "lir/x64/asm/MasmEmitter.h"

void StoreGPEmit::emit(const aasm::GPReg out, const aasm::GPReg in) {
    m_as.mov(m_size, in, aasm::Address(out));
}

void StoreGPEmit::emit(aasm::GPReg out, const aasm::Address &in) {
    unimplemented();
}

void StoreGPEmit::emit(const aasm::Address &out, aasm::GPReg in) {
    unimplemented();
}

void StoreGPEmit::emit(const aasm::Address &out, const aasm::Address &in) {
    unimplemented();
}

void StoreGPEmit::emit(aasm::GPReg out, std::int64_t in) {
    unimplemented();
}

void StoreGPEmit::emit(const aasm::Address &out, std::int64_t in) {
    unimplemented();
}
