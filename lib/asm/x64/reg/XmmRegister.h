#pragma once

#include <cstdint>
#include <string_view>

namespace aasm {
    class XmmRegister final {
    public:
        static constexpr auto NUM_REGISTERS = 16;

        constexpr XmmRegister() noexcept = default;

        constexpr explicit XmmRegister(const std::uint8_t code) noexcept:
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

        constexpr bool operator==(const XmmRegister & other) const {
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

    constexpr XmmRegister xmm0(16);
    constexpr XmmRegister xmm1(17);
    constexpr XmmRegister xmm2(18);
    constexpr XmmRegister xmm3(19);
    constexpr XmmRegister xmm4(20);
    constexpr XmmRegister xmm5(21);
    constexpr XmmRegister xmm6(22);
    constexpr XmmRegister xmm7(23);
    constexpr XmmRegister xmm8(24);
    constexpr XmmRegister xmm9(25);
    constexpr XmmRegister xmm10(26);
    constexpr XmmRegister xmm11(27);
    constexpr XmmRegister xmm12(28);
    constexpr XmmRegister xmm13(29);
    constexpr XmmRegister xmm14(30);
    constexpr XmmRegister xmm15(31);
}