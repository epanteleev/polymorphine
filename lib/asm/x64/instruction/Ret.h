#pragma once

namespace aasm::details {
    class Ret final {
    public:
        friend std::ostream& operator<<(std::ostream &os, const Ret&) {
            return os << "ret";
        }

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::uint8_t RET = 0xC3;
            buffer.emit8(RET);
            return std::nullopt;
        }
    };
}