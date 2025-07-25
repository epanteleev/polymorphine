#include "MovGPEmit.h"

#include "lir/x64/asm/AsmEmitter.h"

void MovGPEmit::emit(const aasm::Address &out, const aasm::GPReg in) {
    m_as.mov(m_size, in, out);
}

void MovGPEmit::emit(const aasm::Address &out, const aasm::Address &in) {
}

void MovGPEmit::emit(const aasm::Address &out, const std::int64_t in) {
    assertion(std::in_range<std::int32_t>(in), "Immediate value for mov must be in range of 32-bit signed integer");
    m_as.mov(m_size, static_cast<std::int32_t>(in), out);
}