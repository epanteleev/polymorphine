#pragma once

#include <cstdint>
#include <array>
#include <string_view>

namespace aasm {
    class GPReg final {
    public:
        static constexpr auto NUMBER_OF_GP_REGS = 16;

        constexpr explicit GPReg(const std::uint8_t code) noexcept: m_code(code) {}
        constexpr GPReg() = default;

        [[nodiscard]]
        constexpr std::uint8_t code() const noexcept {
            return m_code;
        }

        [[nodiscard]]
        constexpr bool is_64_bit_reg() const noexcept {
            const auto coding = static_cast<uint8_t>(m_code);
            return coding >= 8 && coding <= 15;
        }

        constexpr bool operator==(const GPReg & other) const {
            return m_code == other.m_code;
        }

        [[nodiscard]]
        std::string_view name(std::size_t size) const noexcept;

        [[nodiscard]]
        std::size_t hash() const noexcept {
            return m_code;
        }

    private:
        std::uint8_t m_code;
    };

    static constexpr GPReg rax(0);
    static constexpr GPReg rcx(1);
    static constexpr GPReg rdx(2);
    static constexpr GPReg rbx(3);
    static constexpr GPReg rsp(4);
    static constexpr GPReg rbp(5);
    static constexpr GPReg rsi(6);
    static constexpr GPReg rdi(7);
    static constexpr GPReg r8(8);
    static constexpr GPReg r9(9);
    static constexpr GPReg r10(10);
    static constexpr GPReg r11(11);
    static constexpr GPReg r12(12);
    static constexpr GPReg r13(13);
    static constexpr GPReg r14(14);
    static constexpr GPReg r15(15);

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
