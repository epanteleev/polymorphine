#pragma once
#include <cstdint>
#include <variant>

#include "Address.h"

namespace aasm {
    enum ImmType: std::uint8_t {
        IMM_INT,    /* 1, 2, 4 or 8 byte signed number. */
        IMM_ADDR,   /* Symbol-relative address, label etc. */
        IMM_STRING  /* string value, only used for initialization. */
    };

    /*
     * Immediates can be numeric (fit in registers), or references to static
     * string values. Expressions like "hello" + 1 can result in for ex
     * .LC1+1 in GNU assembly.
     */
    struct Imm final {
        [[nodiscard]]
        const long& qword() const {
            return std::get<long>(d);
        }

        [[nodiscard]]
        const int& dword() const {
            return std::get<int>(d);
        }

        [[nodiscard]]
        const short& word() const {
            return std::get<short>(d);
        }

        [[nodiscard]]
        const char& byte() const {
            return std::get<char>(d);
        }

        [[nodiscard]]
        const Address& addr() const {
            return std::get<Address>(d);
        }

        std::uint8_t width;
        std::variant<char, short, int, long, Address> d;
    };
}