#pragma once

#include <ostream>
#include <string_view>

#include "Address.h"
#include "Assembler.h"
#include "Register.h"
#include "utility/Error.h"
#include "Common.h"

namespace aasm {
    template<CodeBuffer C>
    static void add_word_op_size(code& c) {
        c.emit8(Assembler::PREFIX_OPERAND_SIZE);
    }

    class PopR final {
    public:
        PopR(std::uint8_t size, GPReg reg) noexcept
            : m_size(size), m_reg(reg) {}

        std::ostream &operator<<(std::ostream &os) const {
            os << "pop" << prefix_size(m_size) << ' ' << m_reg.name(m_size);
            return os;
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

    private:
        std::uint8_t m_size;
        Address m_addr;
    };
}
