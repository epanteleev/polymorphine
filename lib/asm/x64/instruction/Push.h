#pragma once

namespace aasm::details {
    class PushR final {
    public:
        explicit constexpr PushR(const std::uint8_t size, const GPReg reg)
            : m_size(size), m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream &os, const PushR& pushr);

        template <CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> PUSH_R = {0x50};
            Encoder enc(buffer, PUSH_R, PUSH_R);
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: return enc.encode_O(m_size, m_reg);
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        std::uint8_t m_size;
        GPReg m_reg;
    };

    class PushM final {
    public:
        explicit constexpr PushM(const std::uint8_t size, const Address& addr) noexcept:
            m_addr(addr),
            m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const PushM& pushm);

        template<CodeBuffer buffer>
        [[nodiscard]]
        constexpr std::optional<Relocation> emit(buffer& c) const {
            static constexpr std::array<std::uint8_t, 1> PUSH_M = {0xFF};
            Encoder enc(c, PUSH_M, PUSH_M);
            switch (m_size) {
                case 8: [[fallthrough]];
                case 2: return enc.encode_M_without_REXW(6, m_size, m_addr);
                default: die("Invalid size for push instruction: {}", m_size);
            }
        }

    private:
        const Address m_addr;
        std::uint8_t m_size;
    };

    class PushI final {
    public:
        constexpr PushI(const std::int64_t imm, const std::uint8_t size) noexcept:
            m_imm(imm), m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const PushI& pushi);

        template<CodeBuffer Buffer>
        constexpr std::optional<Relocation> emit(Buffer& buffer) const {
            static constexpr std::array<std::uint8_t, 1> PUSH_IMM = {0x68};
            static constexpr std::array<std::uint8_t, 1> PUSH_IMM_8 = {0x6A};
            Encoder enc(buffer, PUSH_IMM_8, PUSH_IMM);
            switch (m_size) {
                case 1: [[fallthrough]];
                case 2: [[fallthrough]];
                case 4: return enc.encode_I(m_size, m_imm);
                default: die("Invalid size for push instruction: {}", m_size);
            }
        }

    private:
        std::int64_t m_imm;
        std::uint8_t m_size;
    };
}