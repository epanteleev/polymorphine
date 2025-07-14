#pragma once

namespace aasm {
    class Ret final {
    public:
        friend std::ostream& operator<<(std::ostream &os, const Ret& ret);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t RET = 0xC3;
            buffer.emit8(RET);
        }
    };

    inline std::ostream & operator<<(std::ostream &os, const Ret &) {
        return os << "ret";
    }
}