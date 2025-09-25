#pragma once
#include <cstdint>

#include "asm/x64/reg/Register.h"
#include "asm/x64/reg/XmmRegister.h"


namespace aasm::details {
    template<typename Reg>
    requires std::same_as<Reg, GPReg> || std::same_as<Reg, XmmRegister>
    constexpr std::uint8_t B(const Reg arg) noexcept {
        return arg.is_64_bit_reg();
    }

    constexpr std::uint8_t W(const GPReg arg) noexcept {
        return (arg.code() == 8) << 3;
    }

    template<typename Reg>
    requires std::same_as<Reg, GPReg> || std::same_as<Reg, XmmRegister>
    constexpr std::uint8_t R(const Reg arg) noexcept {
        return static_cast<std::uint8_t>(arg.is_64_bit_reg()) << 2;
    }
}