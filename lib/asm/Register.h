#pragma once

#include <cstdint>
#include <array>


class GPReg final {
    enum class RegEncoding: std::uint8_t {
        NONE = 0,
        AX  = 1,    /* 0b000 */
        CX  = 2,    /* 0b001 */
        DX  = 3,    /* 0b010 */
        BX  = 4,    /* 0b011 */
        SP  = 5,    /* 0b100 */
        BP  = 6,    /* 0b101 */
        SI  = 7,    /* 0b110 */
        DI  = 8,    /* 0b111 */
        R8  = 9,
        R9  = 10,
        R10 = 11,
        R11 = 12,
        R12 = 13,
        R13 = 14,
        R14 = 15,
        R15 = 16,
    };

    constexpr explicit GPReg(const RegEncoding code) noexcept: m_code(code) {}
public:

    [[nodiscard]]
    constexpr std::uint8_t code() const noexcept { return static_cast<uint8_t>(m_code); }

    [[nodiscard]]
    constexpr bool is_64_bit_reg() const noexcept {
        return code() >= static_cast<uint8_t>(RegEncoding::R8) && code() <= static_cast<uint8_t>(RegEncoding::R15);
    }

    static consteval GPReg rax() noexcept { return GPReg(RegEncoding::AX); }
    static consteval GPReg rcx() noexcept { return GPReg(RegEncoding::CX); }
    static consteval GPReg rdx() noexcept { return GPReg(RegEncoding::DX); }
    static consteval GPReg rbx() noexcept { return GPReg(RegEncoding::BX); }
    static consteval GPReg rsp() noexcept { return GPReg(RegEncoding::SP); }
    static consteval GPReg rbp() noexcept { return GPReg(RegEncoding::BP); }
    static consteval GPReg rsi() noexcept { return GPReg(RegEncoding::SI); }
    static consteval GPReg rdi() noexcept { return GPReg(RegEncoding::DI); }
    static consteval GPReg r8() noexcept { return GPReg(RegEncoding::R8); }
    static consteval GPReg r9() noexcept { return GPReg(RegEncoding::R9); }
    static consteval GPReg r10() noexcept { return GPReg(RegEncoding::R10); }
    static consteval GPReg r11() noexcept { return GPReg(RegEncoding::R11); }
    static consteval GPReg r12() noexcept { return GPReg(RegEncoding::R12); }
    static consteval GPReg r13() noexcept { return GPReg(RegEncoding::R13); }
    static consteval GPReg r14() noexcept { return GPReg(RegEncoding::R14); }
    static consteval GPReg r15() noexcept { return GPReg(RegEncoding::R15); }

    static constexpr GPReg noreg() noexcept { return GPReg(RegEncoding::NONE); }



private:
    RegEncoding m_code;
};

static constexpr std::array gp_regs = {
    GPReg::rax(), GPReg::rcx(), GPReg::rdx(), GPReg::rbx(),
    GPReg::rsp(), GPReg::rbp(), GPReg::rsi(), GPReg::rdi(),
    GPReg::r8(),  GPReg::r9(),  GPReg::r10(), GPReg::r11(),
    GPReg::r12(), GPReg::r13(), GPReg::r14(), GPReg::r15()
};

static constexpr std::uint8_t B(const GPReg arg) {
    return arg.is_64_bit_reg();
}

static constexpr std::uint8_t W(const GPReg arg) noexcept {
    return (arg.code() == 8) << 3;
}

static constexpr std::uint8_t R(const GPReg arg) noexcept {
    return static_cast<std::uint8_t>(arg.is_64_bit_reg()) << 2;
}

static constexpr std::uint8_t reg3(const GPReg arg) noexcept {
    return (arg.code() - 1) & 0x7;
}

