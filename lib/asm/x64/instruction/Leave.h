#pragma once

namespace aasm::details {
    class Leave final {
    public:
        constexpr Leave() noexcept = default;

        friend std::ostream &operator<<(std::ostream &os, const Leave &leave) {
            return os << "leave";
        }

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            buffer.emit8(0xC9);
            return std::nullopt;
        }
    };
}