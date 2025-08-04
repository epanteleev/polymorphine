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
                    static constexpr std::uint8_t CALL = 0xFF;
                    return details::encode_M<CALL, CALL, 3>(buffer, 8, Address(m_name));
                }
                case Linkage::INTERNAL: {
                    static constexpr std::uint8_t CALL = 0xE8;
                    buffer.emit8(CALL);
                    buffer.emit32(INT32_MAX);
                    return Relocation(RelType::R_X86_64_PC32, buffer.size(), 0, m_name);
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
            static constexpr std::uint8_t CALL = 0xFF;
            buffer.emit8(constants::REX_W);
            buffer.emit8(CALL);
            buffer.emit8(0x15); // CALL r/m64
            buffer.emit64(INT64_MAX);
            return Relocation(RelType::R_X86_64_PLT32, buffer.size(), 0, m_addr.as<AddressLiteral>()->symbol());
        }

    private:
        Address m_addr;
    };

    inline std::ostream & operator<<(std::ostream &os, const CallM &call) {
        return os << "call " << call.m_addr;
    }
}
