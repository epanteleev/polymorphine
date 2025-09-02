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
            switch (m_name->linkage()) {
                case Linkage::EXTERNAL: {
                    static constexpr std::array<std::uint8_t, 1> CALL = {0xFF};
                    Encoder enc(buffer, CALL, CALL);
                    return enc.encode_M(2, 8, Address(m_name));
                }
                case Linkage::INTERNAL: [[fallthrough]];
                case Linkage::DEFAULT: {
                    static constexpr std::uint8_t CALL = 0xE8;
                    buffer.emit8(CALL);
                    buffer.emit32(INT32_MAX);
                    return Relocation(RelType::X86_64_PC32, buffer.size(), 0, m_name);
                }
                default: die("Unsupported linkage type for call: {}", static_cast<std::uint8_t>(m_name->linkage()));
            }
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

    class CallM final {
    public:
        explicit constexpr CallM(const Address& addr) noexcept:
            m_addr(addr) {}

        friend std::ostream &operator<<(std::ostream &os, const CallM &call);

        template<CodeBuffer Buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> CALL = {0xFF};
            Encoder enc(buffer, CALL, CALL);
            return enc.encode_M(2, 8, m_addr);
        }

    private:
        Address m_addr;
    };

    inline std::ostream & operator<<(std::ostream &os, const CallM &call) {
        return os << "call " << call.m_addr;
    }
}
