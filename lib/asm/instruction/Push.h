#pragma once

namespace aasm::details {
    class PushR final {
    public:
        explicit constexpr PushR(const std::uint8_t size, const GPReg reg)
            : m_size(size), m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream &os, const PushR& pushr);

        template <CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t PUSH_R = 0x50;
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: details::encode_O<PUSH_R>(buffer, m_size, m_reg); break;
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        GPReg m_reg;
    };

    inline std::ostream & operator<<(std::ostream &os, const PushR &pushr) {
        return os << "push" << prefix_size(pushr.m_size) << ' ' << pushr.m_reg.name(pushr.m_size);
    }

    class PushM final {
    public:
        explicit constexpr PushM(const std::uint8_t size, const Address& addr) noexcept:
            m_addr(addr),
            m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const PushM& pushm);

        template<CodeBuffer buffer>
        constexpr void emit(buffer& c) const {
            static constexpr std::uint8_t PUSH_M = 0xFF;
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: details::encode_M<PUSH_M, PUSH_M, 0x30>(c, m_size, m_addr); break;
                default: die("Invalid size for push instruction: {}", m_size);
            }
        }

    private:
        const Address m_addr;
        std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &os, const PushM &pushm) {
        return os << "push" << prefix_size(pushm.m_size) << ' ' << pushm.m_addr;
    }

    class PushI final {
    public:
        constexpr PushI(const std::int64_t imm, const std::uint8_t size) noexcept:
            m_imm(imm), m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const PushI& pushi);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            static constexpr std::uint8_t PUSH_IMM = 0x68;
            static constexpr std::uint8_t PUSH_IMM_8 = 0x6A;
            details::encode_I<PUSH_IMM_8, PUSH_IMM>(buffer, m_size, m_imm);
        }

    private:
        std::int64_t m_imm;
        std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &os, const PushI &pushi) {
        return os << "push" << prefix_size(pushi.m_size) << " $" << pushi.m_imm;
    }
}