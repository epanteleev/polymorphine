#pragma once

#include <ostream>
#include <string_view>

#include "Address.h"
#include "Register.h"
#include "utility/Error.h"
#include "Common.h"

namespace aasm {
    template<CodeBuffer Buffer>
    static void add_word_op_size(Buffer& buffer) {
        buffer.emit8(constants::PREFIX_OPERAND_SIZE);
    }

    static char prefix_size(std::uint8_t size) {
        switch (size) {
            case 1: return 'b';
            case 2: return 'w';
            case 4: return 'l';
            case 8: return 'q';
            default: die("Invalid size for prefix: {}", size);
        }
    }

    class PopR final {
    public:
        PopR(const std::uint8_t size, const GPReg reg) noexcept
            : m_size(size), m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream& os, const PopR& popr);

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
        void emit_pop(C& c) const {
            if (const auto rex = constants::REX | B(m_reg); rex != constants::REX) {
                c.emit8(rex);
            } else {
                c.emit8(POP_R + reg3(m_reg));
            }
        }

        std::uint8_t m_size;
        GPReg m_reg;
    };

    inline std::ostream & operator<<(std::ostream &os, const PopR &popr) {
        return os << "pop" << prefix_size(popr.m_size) << ' ' << popr.m_reg.name(popr.m_size);
    }

    class PopM final {
    public:
        PopM(std::uint8_t size, const Address& addr) noexcept:
            m_size(size),
            m_addr(addr) {}

        friend std::ostream& operator<<(std::ostream& os, const PopM& popm);

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
            unsigned char rex = constants::REX | X(m_addr) | B(m_addr.base);
            if (rex != constants::REX) {
                c.emit8(rex);
            }

            c.emit8(POP_M);
            m_addr.encode(c, 0);
        }

        std::uint8_t m_size;
        Address m_addr;
    };

    inline std::ostream & operator<<(std::ostream &os, const PopM &popm) {
        return os << "pop" << prefix_size(popm.m_size) << ' ' << popm.m_addr;
    }

    class PushR final {
    public:
        explicit PushR(std::uint8_t size, const GPReg reg)
            : m_size(size), m_reg(reg) {}

        friend std::ostream& operator<<(std::ostream &os, const PushR& pushr);

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
        void emit_push(C& c) const {
            if (const auto rex = constants::REX | B(m_reg); rex != constants::REX) {
                c.emit8(rex);
            } else {
                c.emit8(PUSH_R + reg3(m_reg));
            }
        }

        std::uint8_t m_size;
        GPReg m_reg;
    };

    inline std::ostream & operator<<(std::ostream &os, const PushR &pushr) {
        return os << "push" << prefix_size(pushr.m_size) << ' ' << pushr.m_reg.name(pushr.m_size);
    }

    class PushM final {
    public:
        explicit PushM(const std::uint8_t size, const Address& addr) noexcept:
            m_addr(addr),
            m_size(size) {}

        friend std::ostream& operator<<(std::ostream &os, const PushM& pushm);

        template<CodeBuffer buffer>
        void emit(buffer& c) const {
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
        void emit_push(C& c) const noexcept {
            unsigned char rex = constants::REX | X(m_addr) | B(m_addr.base);
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

    class MovRR final {
    public:
        explicit MovRR(std::uint8_t size, const GPReg& src, const GPReg& dest) noexcept:
            m_size(size), m_src(src), m_dest(dest) {}

        friend std::ostream& operator<<(std::ostream &os, const MovRR& movrr);

        template<CodeBuffer Buffer>
        void emit(Buffer& buffer) const {
            switch (m_size) {
                case 8: {
                    emit_mov(buffer, constants::REX_W);
                    break;
                }
                case 4: {
                    emit_mov(buffer, 0);
                    break;
                }
                case 2: {
                    add_word_op_size(buffer);
                    emit_mov(buffer, 0);
                    break;
                }
                case 1: {
                    emit_mov_byte(buffer);
                    break;
                }
                default:  die("Invalid size for mov instruction: {}", m_size);
            }
        }

    private:
        static constexpr std::uint8_t MOV_RR = 0x89;
        static constexpr std::uint8_t MOV_RR_8 = 0x88;

        template<CodeBuffer Buffer>
        void emit_mov(Buffer& buffer, const std::uint8_t rex_w) const {
            buffer.emit8(constants::REX | B(m_dest) | rex_w | R(m_src));
            buffer.emit8(MOV_RR);
            buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dest));
        }

        template<CodeBuffer Buffer>
        void emit_mov_byte(Buffer& buffer) const {
            buffer.emit8(constants::REX | B(m_dest) | R(m_src));
            buffer.emit8(MOV_RR_8);
            buffer.emit8(0xC0 | reg3(m_src) << 3 | reg3(m_dest));
        }

        std::uint8_t m_size;
        GPReg m_src;
        GPReg m_dest;
    };

    inline std::ostream & operator<<(std::ostream &os, const MovRR &movrr) {
        return os << "mov" << prefix_size(movrr.m_size) << ' ' << movrr.m_src.name(movrr.m_size) << ", " << movrr.m_dest.name(movrr.m_size);
    }

    class Ret final {
    public:
        friend std::ostream& operator<<(std::ostream &os, const Ret& ret);

        template<CodeBuffer Buffer>
        void emit(Buffer& buffer) const {
            static constexpr std::uint8_t RET = 0xC3;
            buffer.emit8(RET);
        }
    };

    inline std::ostream & operator<<(std::ostream &os, const Ret &) {
        return os << "ret";
    }

    class X64Instruction final {
    public:
        template<typename I>
        explicit X64Instruction(I&& i) noexcept: m_inst(std::forward<I>(i)) {}

        friend std::ostream &operator<<(std::ostream &os, const X64Instruction &inst);

        template<CodeBuffer Buffer>
        void emit(Buffer &buffer) const {
            const auto visitor = [&](const auto &var) {
                var.emit(buffer);
            };

            std::visit(visitor, m_inst);
        }

    private:
        std::variant<PopR, PopM, PushR, PushM, Ret, MovRR> m_inst;
    };

    inline std::ostream &operator<<(std::ostream &os, const X64Instruction &inst) {
        const auto visitor = [&](const auto &var) {
            os << var;
        };

        std::visit(visitor, inst.m_inst);
        return os;
    }
}
