#pragma once

#include <cstdint>
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
        constexpr std::size_t encode() const noexcept {
            return m_code & 0x7;
        }

    private:
        std::uint8_t m_code;
    };

    constexpr GPReg rax(0);
    constexpr GPReg rcx(1);
    constexpr GPReg rdx(2);
    constexpr GPReg rbx(3);
    constexpr GPReg rsp(4);
    constexpr GPReg rbp(5);
    constexpr GPReg rsi(6);
    constexpr GPReg rdi(7);
    constexpr GPReg r8(8);
    constexpr GPReg r9(9);
    constexpr GPReg r10(10);
    constexpr GPReg r11(11);
    constexpr GPReg r12(12);
    constexpr GPReg r13(13);
    constexpr GPReg r14(14);
    constexpr GPReg r15(15);

    constexpr bool is_special_byte_reg(const GPReg arg) {
        return arg == rsi || arg == rdi;
    }
}