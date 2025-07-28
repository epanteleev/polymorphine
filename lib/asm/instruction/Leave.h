#pragma once

namespace aasm {
    class Leave final {
    public:
        constexpr Leave() noexcept = default;

        friend std::ostream &operator<<(std::ostream &os, const Leave &leave);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            buffer.emit8(0xC9); // leave instruction
        }
    };

    inline std::ostream & operator<<(std::ostream &os, const Leave &) {
        return os << "leave";
    }
}
