#pragma once
#include <array>

#include "asm/Register.h"

namespace call_conv {
    static constexpr std::array GP_ARGUMENT_REGISTERS = {
        aasm::rdi,
        aasm::rsi,
        aasm::rdx,
        aasm::rcx,
        aasm::r8,
        aasm::r9
    };

    static constexpr std::array GP_CALLER_SAVE_REGISTERS = {
        aasm::rax,
        aasm::rcx,
        aasm::rdx,
        aasm::rsi,
        aasm::rdi,
        aasm::r8,
        aasm::r9,
        aasm::r10,
        aasm::r11
    };

    static constexpr std::array GP_CALLEE_SAVE_REGISTERS = {
        aasm::rbp,
        aasm::rbx,
        aasm::r12,
        aasm::r13,
        aasm::r14,
        aasm::r15
    };

    static constexpr std::size_t STACK_ALIGNMENT = 16;
}
