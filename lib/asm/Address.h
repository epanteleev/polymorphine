#pragma once

#include <cstdint>
#include <ostream>

#include "Common.h"
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


        friend std::ostream& operator<<(std::ostream & os, const Address & addr);

        template<CodeBuffer C>
        void encode(C& c, unsigned int reg) const {
            /* SP is used as sentinel for SIB, and R12 overlaps. */
            const auto has_sib = index.code() || !base.code() || reg3(base) == reg3(rsp) || scale > 1;

            /* Explicit displacement must be used with BP or R13. */
            const auto has_displacement = !base.code() || displacement || base == rbp;

            // Emit ModR/M byte: [7-6]=mod, [5-3]=reg, [2-0]=r/m or 4 if SIB present
            std::uint8_t modrm = ((reg & 0x7) << 3) | (has_sib ? 4 : reg3(base));

            // Set Mod bits according to displacement presence and range
            if (!in_byte_range(displacement)) {
                // 0b10: 32-bit displacement
                modrm |= 0x80;
            } else if (has_displacement && base.code()) {
                // 0b01: 8-bit displacement
                modrm |= 0x40;
            }
            c.emit8(modrm);

            /* SIB */
            if (has_sib) {
                // SIB: [7-6] scale, [5-3] index, [2-0] base
                std::uint8_t sib = (scale == 2 ? 1 :
                        scale == 4 ? 2 :
                        scale == 8 ? 3 : 0) << 6;
                sib |= (index.code() ? reg3(index) : reg3(rsp)) << 3;
                sib |= base.code() ? reg3(base) : 5;
                c.emit8(sib);
            }

            /* Displacement */
            if (!in_byte_range(displacement) || !base.code()) {
                c.emit32(displacement);
            } else if (has_displacement) {
                c.emit8(displacement);
            }
        }

        AddressType type;

        int displacement;

        GPReg base;
        GPReg index;
        std::uint8_t scale;
    };

    inline std::ostream& operator<<(std::ostream & os, const Address & addr) {
        if (addr.displacement != 0) {
            os << addr.displacement;
        }
        os << '(';
        if (addr.base.code()) {
            os << addr.base.name(8);
        }
        if (addr.index.code()) {
            if (addr.base.code()) {
                os << ',';
            }
            os << addr.index.name(8);
            if (addr.scale != 1) {
                os << ',' << static_cast<int>(addr.scale);
            }
        }

        return os;
    }

    static constexpr std::uint8_t X(const Address& arg) {
        return arg.index.is_64_bit_reg() << 1;
    }
}