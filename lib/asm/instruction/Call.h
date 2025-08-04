#pragma once

namespace aasm::details {
    class Call final {
    public:
        explicit constexpr Call(const Symbol* name) noexcept:
            m_name(name) {}

        friend std::ostream& operator<<(std::ostream &os, const Call& call);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::uint8_t CALL = 0xE8;
            buffer.emit8(CALL);
            buffer.emit32(INT32_MAX);
            return Relocation(RelType::R_X86_64_PC32, buffer.size(), 0, m_name);
        }

        [[nodiscard]]
        const Symbol* name() const noexcept {
            return m_name;
        }

    private:
        const Symbol* m_name;
    };

    inline std::ostream &operator<<(std::ostream &os, const Call &call) {
        return os << "call " << call.m_name->name();
    }
}