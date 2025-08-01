#pragma once

namespace aasm::details {
    class PushR final {
    public:
        explicit constexpr PushR(const std::uint8_t size, const GPReg reg)
            : m_size(size), m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream &os, const PushR& pushr);

        template <CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            switch (m_size) {
                case 8: {
                    emit_push(buffer);
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    emit_push(buffer);
                    break;
                }
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t PUSH_R = 0x50;

        template<CodeBuffer C>
        constexpr void emit_push(C& c) const {
            if (const auto rex = constants::REX | B(m_reg); rex != constants::REX) {
                c.emit8(rex);
            }
            c.emit8(PUSH_R + reg3(m_reg));
        }

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
            switch (m_size) {
                case 8: {
                    emit_push(c);
                    break;
                }
                case 2: {
                    add_word_op_size(c);
                    emit_push(c);
                    break;
                }
                default: die("Invalid size for push instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t PUSH_M = 0xFF;

        template<CodeBuffer C>
        constexpr void emit_push(C& c) const noexcept {
            unsigned char rex = constants::REX | X(m_addr) | B(m_addr.base());
            if (rex != constants::REX) {
                c.emit8(rex);
            }

            c.emit8(PUSH_M);
            m_addr.encode(c, 6);
        }

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
            switch (m_size) {
                case 4: {
                    buffer.emit8(PUSH_IMM);
                    buffer.emit32(checked_cast<std::int32_t>(m_imm));
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    buffer.emit8(PUSH_IMM);
                    buffer.emit16(checked_cast<std::int16_t>(m_imm));
                    break;
                }
                case 1: {
                    buffer.emit8(PUSH_IMM_8);
                    buffer.emit8(checked_cast<std::int8_t>(m_imm));
                    break;
                }
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t PUSH_IMM = 0x68;
        static constexpr std::uint8_t PUSH_IMM_8 = 0x6A;

        std::int64_t m_imm;
        std::uint8_t m_size;
    };

    inline std::ostream & operator<<(std::ostream &os, const PushI &pushi) {
        return os << "push" << prefix_size(pushi.m_size) << " $" << pushi.m_imm;
    }
}
