#include "Register.h"

#include "utility/Error.h"

namespace aasm {
    std::string_view GPReg::name(std::size_t size) const noexcept {
        static const char* reg_names8[] = {
            "none", "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
            "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15"
        };

        static const char* reg_names4[] = {
            "none", "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi",
            "r8d",  "r9d",  "r10d", "r11d", "r12d", "r13d", "r14d", "r15d"
        };

        static const char* reg_names2[] = {
            "none", "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
            "r8w",  "r9w",  "r10w", "r11w", "r12w", "r13w", "r14w", "r15w"
        };

        static const char *reg_names1[] = {
            "none", "al", "cl", "dl", "bl", "spi", "bpl", "sil", "dil",
            "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
        };

        const auto idx = static_cast<std::size_t>(m_code);
        switch (size) {
            case 1: return reg_names1[idx];
            case 2: return reg_names2[idx];
            case 4: return reg_names4[idx];
            case 8: return reg_names8[idx];
            default: die("Invalid register size: {}", size);
        }
    }
}
