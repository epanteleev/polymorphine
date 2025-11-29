#pragma once

#include <array>
#include "asm/x64/reg/GPReg.h"
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

        static constexpr std::array XMM_ARGUMENT_REGISTERS = {
            aasm::xmm0,
            aasm::xmm1,
            aasm::xmm2,
            aasm::xmm3,
            aasm::xmm4,
            aasm::xmm5,
            aasm::xmm6,
            aasm::xmm7
        };

        static constexpr aasm::GPRegSet GP_CALLER_SAVE_REGISTERS{
            {
                aasm::rax,
                aasm::rcx,
                aasm::rdx,
                aasm::rsi,
                aasm::rdi,
                aasm::r8,
                aasm::r9,
                aasm::r10,
                aasm::r11
            }
        };

        static constexpr aasm::XmmRegSet XMM_CALLER_SAVE_REGISTERS = {
            {
                aasm::xmm0,
                aasm::xmm1,
                aasm::xmm2,
                aasm::xmm3,
                aasm::xmm4,
                aasm::xmm5,
                aasm::xmm6,
                aasm::xmm7,
                aasm::xmm8,
                aasm::xmm9,
                aasm::xmm10,
                aasm::xmm11,
                aasm::xmm12,
                aasm::xmm13,
                aasm::xmm14,
                aasm::xmm15
            }
        };

        static constexpr aasm::GPRegSet GP_CALLEE_SAVE_REGISTERS = {
            {
                aasm::rbp,
                aasm::rbx,
                aasm::r12,
                aasm::r13,
                aasm::r14,
                aasm::r15
            }
        };

        static constexpr aasm::GPRegSet ALL_GP_REGISTERS {
            {
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
            }
        };

        static constexpr aasm::XmmRegSet ALL_XMM_REGISTERS = {
            {
                aasm::xmm0,
                aasm::xmm1,
                aasm::xmm2,
                aasm::xmm3,
                aasm::xmm4,
                aasm::xmm5,
                aasm::xmm6,
                aasm::xmm7,
                aasm::xmm8,
                aasm::xmm9,
                aasm::xmm10,
                aasm::xmm11,
                aasm::xmm12,
                aasm::xmm13,
                aasm::xmm14,
                aasm::xmm15
            }
        };

        static constexpr CallConvProvider CC_LinuxX64_instance{GP_ARGUMENT_REGISTERS,
            GP_CALLER_SAVE_REGISTERS,
            GP_CALLEE_SAVE_REGISTERS,
            ALL_GP_REGISTERS,
            XMM_ARGUMENT_REGISTERS,
            XMM_CALLER_SAVE_REGISTERS,
            {},
            ALL_XMM_REGISTERS
        };
    };

    static consteval const CallConvProvider* CC_LinuxX64() {
        return &LinuxX64::CC_LinuxX64_instance;
    }
};