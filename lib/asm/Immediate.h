#pragma once

#include <cstdint>
#include <variant>

namespace aasm {
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

        std::uint8_t width;
        std::variant<char, short, int, long> d;
    };
}