#pragma once

#include <cstdint>
#include <string_view>

namespace aasm {
    class XmmReg final {
    public:
        static constexpr auto NUM_REGISTERS = 16;

        constexpr XmmReg() noexcept = default;

        constexpr explicit XmmReg(const std::uint8_t code) noexcept:
            m_code(code) {}

        [[nodiscard]]
        constexpr std::uint8_t code() const noexcept {
            return m_code;
        }

        [[nodiscard]]
        constexpr bool is_64_bit_reg() const noexcept {
            const auto coding = static_cast<uint8_t>(m_code);
            return coding >= 24; // xmm8 and above.
        }

        constexpr bool operator==(const XmmReg & other) const {
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

    constexpr XmmReg xmm0(16);
    constexpr XmmReg xmm1(17);
    constexpr XmmReg xmm2(18);
    constexpr XmmReg xmm3(19);
    constexpr XmmReg xmm4(20);
    constexpr XmmReg xmm5(21);
    constexpr XmmReg xmm6(22);
    constexpr XmmReg xmm7(23);
    constexpr XmmReg xmm8(24);
    constexpr XmmReg xmm9(25);
    constexpr XmmReg xmm10(26);
    constexpr XmmReg xmm11(27);
    constexpr XmmReg xmm12(28);
    constexpr XmmReg xmm13(29);
    constexpr XmmReg xmm14(30);
    constexpr XmmReg xmm15(31);
}