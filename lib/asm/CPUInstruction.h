#pragma once

#include <ostream>
#include <string_view>

#include "Address.h"
#include "Register.h"
#include "utility/Error.h"
#include "Common.h"

namespace aasm {
    template<CodeBuffer C>
    static void add_word_op_size(C& c) {
        c.emit8(Assembler::PREFIX_OPERAND_SIZE);
    }

    class PopR final {
    public:
        PopR(std::uint8_t size, GPReg reg) noexcept
            : m_size(size), m_reg(reg) {}

        std::ostream &operator<<(std::ostream &os) const {
            return os << "pop" << prefix_size(m_size) << ' ' << m_reg.name(m_size);
        }

        template<CodeBuffer C>
        void emit(C &c) const {
            switch (m_size) {
                case 8: emit_pop(c); break;
                case 2: {
                    add_word_op_size(c);
                    emit_pop(c);
                    break;
                }
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t POP_R = 0x58;

        template<CodeBuffer C>
        void emit_pop(C& c) {
            if (const auto rex = constants::REX | B(m_reg); rex != constants::REX) {
                c.emit8(rex);
            } else {
                c.emit8(POP_R + reg3(m_reg));
            }
        }

        std::uint8_t m_size;
        GPReg m_reg;
    };

    class PopM final {
    public:
        PopM(std::uint8_t size, const Address& addr) noexcept:
            m_size(size),
            m_addr(addr) {}

        std::ostream &operator<<(std::ostream &os) const {
            return os << "pop" << prefix_size(m_size) << ' ' << m_addr;
        }

        template<CodeBuffer C>
        void emit(C &c) const {
            switch (m_size) {
                case 8: emit_pop(c);
                    break;
                case 2: {
                    add_word_op_size(c);
                    emit_pop(c);
                    break;
                }
                default: die("Invalid size for pop instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t POP_M = 0x8F;

        template<CodeBuffer C>
        void emit_pop(C& c) const noexcept {
            unsigned char rex = Assembler::REX | X(m_addr) | B(m_addr.base);
            if (rex != Assembler::REX) {
                c.emit8(rex);
            }

            c.emit8(POP_M);
            m_addr.encode(c, 0);
        }

        std::uint8_t m_size;
        Address m_addr;
    };


    class PushR final {
    public:
        explicit PushR(const GPReg reg, std::uint8_t size)
            : m_reg(reg), m_size(size) {}

        std::ostream &operator<<(std::ostream &os) const {
            return os << "push" << prefix_size(m_size) << ' ' << m_reg.name(m_size);
        }

        template <CodeBuffer Buffer>
        void emit(Buffer& buffer) const {
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
        void emit_push(C& c) {
            const auto rex = Assembler::REX | B(m_reg);
            if (rex != Assembler::REX) {
                c.emit8(rex);
            } else {
                c.emit8(PUSH_R + reg3(m_reg));
            }
        }

        GPReg m_reg;
        std::uint8_t m_size;
    };

    class PushM final {
    public:
        explicit PushM(const Address& addr, const std::uint8_t size) noexcept:
            m_size(size),
            m_addr(addr) {}

        std::ostream &operator<<(std::ostream &os) const {
            return os << "push" << prefix_size(m_size) << ' ' << m_addr;
        }

        template<CodeBuffer buffer>
        void emit(buffer& c) const {
            switch (m_size) {
                case 8: emit_push(c);
                    break;
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
        void emit_push(C& c) const noexcept {
            unsigned char rex = Assembler::REX | X(m_addr) | B(m_addr.base);
            if (rex != Assembler::REX) {
                c.emit8(rex);
            }

            c.emit8(PUSH_M);
            m_addr.encode(c, 0);
        }

        std::uint8_t m_size;
        const Address m_addr;
    };
}
