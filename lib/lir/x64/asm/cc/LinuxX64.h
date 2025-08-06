#pragma once

#include <array>
#include "../../../../asm/reg/Register.h"
#include "CallConv.h"

namespace call_conv {
    class LinuxX64 final {
    public:
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

        static constexpr std::array ALL_GP_REGISTERS = {
            aasm::rax,
            aasm::rbx,
            aasm::rcx,
            aasm::rdx,
            aasm::rdi,
            aasm::rsi,
            //aasm::rbp, Exclude rbp from the available registers, it is used for stack frame pointer.
            aasm::rsp,
            aasm::r8,
            aasm::r9,
            aasm::r10,
            aasm::r11,
            aasm::r12,
            aasm::r13,
            aasm::r14,
            aasm::r15
        };
    };

    static_assert(CallConv<LinuxX64>, "LinuxX64 must satisfy CallConv concept");
};