#include "Assembler.h"

#include <cstring>
#include <iostream>
#include <ranges>
#include <spanstream>

#include "Instruction.h"


namespace aasm {
    namespace {

        void add_word_op_size(code& c) {
            c.val[c.len++] = PREFIX_OPERAND_SIZE;
        }

        int encode_modrm_sib_disp(code *c, unsigned int reg, const Address& addr) {
            const int len = c->len;

            //  assert(!addr.sym);
            // assert(addr.base || !addr.displacement);


            /* SP is used as sentinel for SIB, and R12 overlaps. */
            int has_sib = addr.index.code() || !addr.base.code() || reg3(addr.base) == reg3(SP) || addr.scale > 1;

            /* Explicit displacement must be used with BP or R13. */
            int has_displacement = !addr.base.code() || addr.displacement || reg3(addr.base) == reg3(BP);

            /* ModR/M */
            c->val[c->len++] = (reg & 0x7) << 3 | (has_sib ? 4 : reg3(addr.base));
            if (!in_byte_range(addr.displacement)) {
                c->val[c->len - 1] |= 0x80;
            } else if (has_displacement && addr.base.code()) {
                c->val[c->len - 1] |= 0x40;
            }

            /* SIB */
            if (has_sib) {
                c->val[c->len] = addr.index.code() ? reg3(addr.index) : reg3(SP);
                c->val[c->len] = c->val[c->len] << 3;
                c->val[c->len] |= (
                    addr.scale == 2 ? 1 :
                    addr.scale == 4 ? 2 :
                    addr.scale == 8 ? 3 : 0) << 6;
                c->val[c->len] |= addr.base.code() ? reg3(addr.base) : 5;
                c->len++;
            }

            /* Displacement */
            if (!in_byte_range(addr.displacement) || !addr.base.code()) {
                std::memcpy(&c->val[c->len], &addr.displacement, 4);
                c->len += 4;
            } else if (has_displacement) {
                c->val[c->len++] = addr.displacement;
            }

            return c->len - len;
        }

        void emit_pop(code& c, const GPReg r) {
            const auto rex = Assembler::REX | B(r);
            if (rex != Assembler::REX) {
                c.val[c.len++] = rex;
            } else {
                c.val[c.len++] = opc::POP_R + reg3(r);
            }
        }

        void emit_pop(code& c, const Address& addr) {
            unsigned char rex = Assembler::REX | X(addr) | B(addr.base);
            if (rex != Assembler::REX) {
                c.val[c.len++] = rex;
            }

            c.val[c.len++] = opc::POP_M; // POP instruction opcode
            encode_modrm_sib_disp(&c, 0, addr); // Updated function call
        }

        void emit_push(code& c, const GPReg r) {
            const auto rex = Assembler::REX | B(r);
            if (rex != Assembler::REX) {
                c.val[c.len++] = rex;
            } else {
                c.val[c.len++] = opc::PUSH_R + reg3(r); // PUSH instruction opcode
            }
        }

        void emit_push(code& c, const Address& addr) {
            unsigned char rex = Assembler::REX | X(addr) | B(addr.base);
            if (rex != Assembler::REX) {
                c.val[c.len++] = rex;
            }

            c.emit8(opc::PUSH_M);
            encode_modrm_sib_disp(&c, 6, addr); // Updated function call
        }

        void emit_mov(code& c, std::uint8_t rex_w, const GPReg src, const GPReg dest) {
            c.emit8(Assembler::REX | B(dest) | rex_w | R(src));
            c.emit8(opc::MOV_RR);
            c.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
        }
    }


    void Assembler::ret() {
        // Ret - return from function
        m_inst.emplace_back(opc::RET);
    }

    void Assembler::popq(const GPReg r) {
        auto& c = m_inst.emplace_back();
        emit_pop(c, r);
    }

    void Assembler::popw(const GPReg r) {
        auto& c = m_inst.emplace_back();
        add_word_op_size(c);
        emit_pop(c, r);
    }

    void Assembler::popq(const Address& addr) {
        auto& c = m_inst.emplace_back();
        emit_pop(c, addr);
    }

    void Assembler::popw(const Address &addr) {
        auto& c = m_inst.emplace_back();
        add_word_op_size(c);
        emit_pop(c, addr);
    }

    void Assembler::pushq(const GPReg r) {
        auto& c = m_inst.emplace_back();
        emit_push(c, r);
    }

    void Assembler::pushw(const GPReg r) {
        auto& c = m_inst.emplace_back();
        add_word_op_size(c);
        emit_push(c, r);
    }

    void Assembler::pushq(const Address &addr) {
        auto &c = m_inst.emplace_back();
        emit_push(c, addr);
    }

    void Assembler::pushw(const Address &addr) {
        auto &c = m_inst.emplace_back();
        add_word_op_size(c);
        emit_push(c, addr);
    }

    void Assembler::movq(const GPReg src, const GPReg dest) {
        auto &c = m_inst.emplace_back();
        emit_mov(c, REX_W, src, dest);
    }

    void Assembler::movl(const GPReg src, const GPReg dest) {
        auto &c = m_inst.emplace_back();
        c.emit8(REX | B(dest) | R(src));
        c.emit8(opc::MOV_RR);
        c.val[c.len++] = 0xC0 | reg3(src) << 3 | reg3(dest);
    }

    void Assembler::movw(const GPReg src, const GPReg dest) {
        auto& c = m_inst.emplace_back();
        add_word_op_size(c);
        emit_mov(c, 0, src, dest);
    }

    void Assembler::movb(const GPReg src, const GPReg dest) {
        auto &c = m_inst.emplace_back();
        c.emit8(REX | B(dest) | R(src));
        c.emit8(opc::MOV_RR_8);
        c.emit8(0xC0 | reg3(src) << 3 | reg3(dest));
    }

    void Assembler::print_codes(std::ostream &os) const {
        for (const auto& c : m_inst) {
            for (const auto i : std::views::iota(0U, c.len)) {
                os << std::hex << static_cast<int>(c.val[i]) << ' ';
            }

            os << std::endl;
        }
    }

    std::size_t Assembler::to_byte_buffer(std::span<std::uint8_t> buffer) const {
        std::size_t size = 0;
        auto it = buffer.begin();
        for (const auto& c : m_inst) {
            if (it + c.len > buffer.end()) {
                return size;
            }

            std::copy_n(c.val, c.len, it);
            it += c.len;
            size += c.len;
        }

        return size;
    }
}
