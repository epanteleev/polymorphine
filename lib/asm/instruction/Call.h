#pragma once

namespace aasm::details {
    class Call final {
    public:
        explicit constexpr Call(std::string_view name) noexcept:
            m_name(name) {}

        friend std::ostream& operator<<(std::ostream &os, const Call& call);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t CALL = 0xE8;
            buffer.emit8(CALL);
            buffer.emit32(INT32_MAX);
        }

        [[nodiscard]]
        std::string_view name() const noexcept {
            return m_name;
        }

    private:
        std::string m_name;
    };

    inline std::ostream &operator<<(std::ostream &os, const Call &call) {
        return os << "call " << call.m_name;
    }
}