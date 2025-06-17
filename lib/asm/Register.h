#pragma once

#include <cstdint>
#include <array>

namespace aasm {
    class GPReg final {
    public:
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

        [[nodiscard]]
        constexpr std::uint8_t code() const noexcept { return static_cast<uint8_t>(m_code); }

        [[nodiscard]]
        constexpr bool is_64_bit_reg() const noexcept {
            return code() >= static_cast<uint8_t>(RegEncoding::R8) && code() <= static_cast<uint8_t>(RegEncoding::R15);
        }

        static constexpr GPReg noreg() noexcept { return GPReg(RegEncoding::NONE); }

    private:
        RegEncoding m_code;
    };

    static constexpr GPReg rax(GPReg::RegEncoding::AX);
    static constexpr GPReg rbx(GPReg::RegEncoding::BX);
    static constexpr GPReg rsi(GPReg::RegEncoding::SI);
    static constexpr GPReg rdi(GPReg::RegEncoding::DI);
    static constexpr GPReg rcx(GPReg::RegEncoding::CX);
    static constexpr GPReg rdx(GPReg::RegEncoding::DX);
    static constexpr GPReg rbp(GPReg::RegEncoding::BP);
    static constexpr GPReg rsp(GPReg::RegEncoding::SP);
    static constexpr GPReg r8(GPReg::RegEncoding::R8);
    static constexpr GPReg r9(GPReg::RegEncoding::R9);
    static constexpr GPReg r10(GPReg::RegEncoding::R10);
    static constexpr GPReg r11(GPReg::RegEncoding::R11);
    static constexpr GPReg r12(GPReg::RegEncoding::R12);
    static constexpr GPReg r13(GPReg::RegEncoding::R13);
    static constexpr GPReg r14(GPReg::RegEncoding::R14);
    static constexpr GPReg r15(GPReg::RegEncoding::R15);

    static constexpr std::array gp_regs = {
        rax, rcx, rdx, rbx,
        rsp, rbp, rsi, rdi,
        r8,  r9,  r10, r11,
        r12, r13, r14, r15
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
}
