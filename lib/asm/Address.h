#pragma once
#include <cstdint>

#include "Register.h"

namespace aasm {
    enum class AddressType: std::uint8_t {
        ADDR_NORMAL,
        ADDR_GLOBAL_OFFSET,
        ADDR_PLT
    };

    class Address final {

    public:
        explicit constexpr Address(const GPReg base, const GPReg index, std::uint8_t scale = 1, int displacement = 0): type(AddressType::ADDR_NORMAL),
            displacement(displacement),
            base(base),
            index(index),
            scale(scale) {}

        AddressType type;

        int displacement;

        GPReg base;
        GPReg index;
        std::uint8_t scale;
    };


    static constexpr std::uint8_t X(const Address& arg) {
        return arg.index.is_64_bit_reg() << 1;
    }
}