#pragma once

#include <cstdint>
#include <array>
#include <string_view>

namespace aasm {
    class GPReg final {
    public:
        enum class RegEncoding: std::uint8_t {
            AX  = 0,    /* 0b000 */
            CX  = 1,    /* 0b001 */
            DX  = 2,    /* 0b010 */
            BX  = 3,    /* 0b011 */
            SP  = 4,    /* 0b100 */
            BP  = 5,    /* 0b101 */
            SI  = 6,    /* 0b110 */
            DI  = 7,    /* 0b111 */
            R8  = 8,
            R9  = 9,
            R10 = 10,
            R11 = 11,
            R12 = 12,
            R13 = 13,
            R14 = 14,
            R15 = 15,
        };

        static constexpr auto NUMBER_OF_GP_REGS = 16;

        constexpr explicit GPReg(const RegEncoding code) noexcept: m_code(code) {}
        constexpr GPReg() = default;

        [[nodiscard]]
        constexpr std::uint8_t code() const noexcept {
            return static_cast<uint8_t>(m_code);
        }

        [[nodiscard]]
        constexpr bool is_64_bit_reg() const noexcept {
            const auto coding = static_cast<uint8_t>(m_code);
            return coding >= static_cast<uint8_t>(RegEncoding::R8) && coding <= static_cast<uint8_t>(RegEncoding::R15);
        }

        constexpr bool operator==(const GPReg & other) const {
            return m_code == other.m_code;
        }

        [[nodiscard]]
        std::string_view name(std::size_t size) const noexcept;

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
        return arg.code() & 0x7;
    }

    static constexpr bool is_special_byte_reg(const GPReg arg) {
        return arg == rsi || arg == rdi;
    }
}
