#include "Mem2Reg.h"

void Mem2Reg::run(FunctionData &fn) noexcept {
    Mem2Reg mem2_reg(fn);
    mem2_reg.run_impl();
}

void Mem2Reg::run_impl() noexcept {
}
