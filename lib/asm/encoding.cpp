
#include "encoding.h"

#include <algorithm>
#include <cstring>
#include <cassert>
#include <exception>
#include <vector>
static constexpr std::uint8_t REX = 0x40;
/*
 * Some instructions have fixed value of one operand, determined by the
 * opcode.
 *
 * Example: Shift with register->register only works with %cl as source,
 *          thus 'shl %cl, %rax' does not encode %cl explicitly.
 */
enum implicit: std::uint8_t {
    IMPL_NONE = 0,
    IMPL_CX = 1,
    IMPL_AX = 2
};

std::vector<encoding> encodings = {
    {opcodes::INSTR_ADD, {"add", 1}, {0}, {0x00}, OPX_DW, 0x00, OPT_REG_REG | OPT_MEM_REG | OPT_REG_MEM},
    {INSTR_ADD, {"add", 1}, {0}, {0x80}, OPX_SW, 0x00, OPT_IMM_REG | OPT_IMM_MEM, {0}, 0, 1},
    {INSTR_RET, {"ret"}, {0}, {0xC3}, OPX_NONE, 0x00, OPT_NONE},
};

static int encode_modrm_sib_disp(code *c, unsigned int reg, address addr) {
    const int len = c->len;

  //  assert(!addr.sym);
   // assert(addr.base || !addr.displacement);

    /* Scale is 2 bits, representing 1, 2, 4 or 8. */
    assert(addr.scale == 0
        || addr.scale == 1
        || addr.scale == 2
        || addr.scale == 4
        || addr.scale == 8);

    /* SP is used as sentinel for SIB, and R12 overlaps. */
    int has_sib = addr.index || !addr.base || reg3(addr.base) == reg3(SP) || addr.scale > 1;

    /* Explicit displacement must be used with BP or R13. */
    int has_displacement = !addr.base || addr.displacement || reg3(addr.base) == reg3(BP);

    /* ModR/M */
    c->val[c->len++] = (reg & 0x7) << 3 | (has_sib ? 4 : reg3(addr.base));
    if (!in_byte_range(addr.displacement)) {
        c->val[c->len - 1] |= 0x80;
    } else if (has_displacement && addr.base) {
        c->val[c->len - 1] |= 0x40;
    }

    /* SIB */
    if (has_sib) {
        c->val[c->len] = (addr.index) ? reg3(addr.index) : reg3(SP);
        c->val[c->len] = c->val[c->len] << 3;
        c->val[c->len] |= (
            addr.scale == 2 ? 1 :
            addr.scale == 4 ? 2 :
            addr.scale == 8 ? 3 : 0) << 6;
        c->val[c->len] |= addr.base ? reg3(addr.base) : 5;
        c->len++;
    }

    /* Displacement */
    if (!in_byte_range(addr.displacement) || !addr.base) {
        std::memcpy(&c->val[c->len], &addr.displacement, 4);
        c->len += 4;
    } else if (has_displacement) {
        c->val[c->len++] = addr.displacement;
    }

    return c->len - len;
}


/*
 * Encode address using ModR/M, SIB and Displacement bytes. Based on
 * Table 2.2 and Table 2.3 in reference manual. Symbol references are
 * encoded as %rip- relative addresses, section 2.2.1.6.
 *
 * Addend is to account for additional bytes in the instruction after
 * relocation offset is written.
 */
static int encode_address(code *c, unsigned int reg, address addr, int addend) {

    return encode_modrm_sib_disp(c, reg, addr);
    //if (addr.sym) {
    //    assert(false);
        /*c->val[c->len++] = ((reg & 0x7) << 3) | 0x5;
        if (addr.type == ADDR_GLOBAL_OFFSET) {
            reloc = R_X86_64_GOTPCREL;
        } else {
            assert(addr.type == ADDR_NORMAL);
            reloc = R_X86_64_PC32;
        }

        elf_add_relocation(section.rela_text,
            addr.sym, reloc, c->len, addr.displacement - addend);
        memset(&c->val[c->len], 0, 4);
        c->len += 4;
        return 5;*/
   // } else {
  //      return encode_modrm_sib_disp(c, reg, addr);
  //  }
}

/*
 * In 64-bit mode, all instructions have default operand size of 32 bits
 * except branches and instructions like push/pop which implicitly
 * references stack pointer.
 */
static int default_operand_size(opcodes opc)
{
    switch (opc) {
        case INSTR_CALL:
        case INSTR_PUSH:
        case INSTR_POP:
        case INSTR_JMP:
        case INSTR_Jcc:
            return 8;
        default:
            return 4;
    }
}

static void encode_opcode(code *c, encoding enc) {
    int i;

    c->val[c->len++] = enc.opcode[0];
    for (i = 1; enc.opcode[i] && i < 2; ++i) {
        c->val[c->len++] = enc.opcode[i];
    }
}


static void encode_reg_reg(code *c, encoding enc, int ws, int w, reg a, reg b) {
    unsigned char rex = REX | B(b);

    if (enable_rex_w(enc.opc) && w != default_operand_size(enc.opc)) {
        rex = rex | W(w);
    }

    assert(!enc.openc[1].implicit);
    if (!enc.openc[0].implicit) {
        rex |= R(a);
    }

    if (rex != REX) {
        c->val[c->len++] = rex;
    }

    encode_opcode(c, enc);
    if ((enc.opextra & OPX_W) == OPX_W) {
        if (enc.openc[0].implicit) {
            ws = w;
        }
        c->val[c->len - 1] |= ws != 1;
    }

    c->val[c->len++] = 0xC0 | enc.modrm | reg3(b);
    if (!enc.openc[0].implicit) {
        assert(!enc.modrm);
        c->val[c->len - 1] |= reg3(a) << 3;
    }
}


static void encode_mem_reg(code *c, encoding enc, int w, int d, memory mem, reg b) {
    unsigned char rex = REX | R(b) | X(mem.addr) | B(mem.addr.base);
    if (enable_rex_w(enc.opc) && w != default_operand_size(enc.opc)) {
        rex = rex | W(w);
    }

    assert(!enc.openc[1].implicit);
    if (rex != REX || ((b == SI || b == DI) && w == 1)) {
        c->val[c->len++] = rex;
    }

    encode_opcode(c, enc);

    /* Specify full width in last opcode bit. */
    if ((enc.opextra & OPX_W) == OPX_W) {
        c->val[c->len - 1] |= mem.width != 1;
    }

    /* Direction bit, determine source and destination. */
    if ((enc.opextra & OPX_D) == OPX_D) {
        c->val[c->len - 1] |= (d == 1) << 1;
    }

    encode_address(c, reg3(b), mem.addr, 0);
}


static int encode_immediate(code *c,immediate imm, int sx_byte_offset, int is_displacement_or_dword) {
    switch (imm.type) {
        case IMM_INT: {
            if (imm.width == 1 || (sx_byte_offset && in_byte_range(imm.qword()))) {
                if (sx_byte_offset) {
                    c->val[c->len - sx_byte_offset] |= 0x02;
                }
                c->val[c->len++] = imm.byte();
            } else if (imm.width == 2) {
                memcpy(c->val + c->len, &imm.word(), 2);
                c->len += 2;
            } else if (imm.width == 4 || is_displacement_or_dword) {
                assert(imm.width == 4 || in_32bit_range(imm.qword()));
                memcpy(c->val + c->len, &imm.dword(), 4);
                c->len += 4;
            } else {
                assert(imm.width == 8);
                memcpy(c->val + c->len, &imm.qword(), 8);
                c->len += 8;
            }
            break;
        }
        case IMM_ADDR: {
            address addr = imm.addr();
           // assert(addr.sym);
            /*
            if (is_displacement_or_dword) {
                assert(addr.type == ADDR_NORMAL);
                disp = elf_text_displacement(addr.sym, c->len) + addr.displacement - 4;
                memcpy(c->val + c->len, &disp, 4);
            } else {
                assert(addr.type == ADDR_NORMAL || addr.type == ADDR_PLT);
                reloc = addr.type == ADDR_NORMAL ? R_X86_64_PC32 : R_X86_64_PLT32;
                elf_add_relocation(section.rela_text,
                    addr.sym, reloc, c->len, addr.displacement);
            }
            */
            assert(false);
            c->len += 4;
            break;
        }
        case IMM_STRING:
            assert(false);
            break;
        default:
            std::terminate();
    }
    int len = c->len;
    return c->len - len;
}


static void encode_imm_reg(code *c, const encoding &enc, int w, const immediate &a, const reg b) {
    int d;
    unsigned char rex;

    rex = REX | W(w) | B(b);
    if (rex != REX || ((b == SI || b == DI) && w == 1)) {
        c->val[c->len++] = rex;
    }

    encode_opcode(c, enc);
    if ((enc.opextra & OPX_W) == OPX_W) {
        w = w != 1;
        if ((enc.opextra & OPX_REG) == OPX_REG) {
            w <<= 3;
        }

        c->val[c->len - 1] |= w;
    }

    if ((enc.opextra & OPX_REG) == OPX_REG) {
        assert(!enc.modrm);
        assert(!enc.openc[1].implicit);
        c->val[c->len - 1] |= reg3(b);
        d = 1;
    } else if (!enc.openc[1].implicit) {
        c->val[c->len++] = 0xC0 | enc.modrm | reg3(b);
        d = 2;
    } else {
        d = 0;
    }

    /*
     * Set zero if not supporting 1 byte immediate, or if explicitly 1
     * byte operand.
     */
    if (((enc.opextra & OPX_S) != OPX_S) || w == 0) {
        d = 0;
    }

    encode_immediate(c, a, d, enc.is_displacement_or_dword);
}


static int imm_encoding_width(const immediate& imm, const int allow_sign_extend, int max_32bit) {
    switch (imm.type) {
        case IMM_INT: {
            if (imm.width == 1
                || (allow_sign_extend && in_byte_range(imm.qword())))
            {
                return 1;
            }
            if (imm.width == 2) {
                return 2;
            }
            if (imm.width == 4 || max_32bit) {
                return 4;
            }
            return 8;
        }
        break;
        case IMM_ADDR:
            return 4;
        default:
            return 0;
    }
}


static void encode_imm_mem(code *c, const encoding &enc, int w, const immediate& a, const address &b) {
    unsigned char rex = REX | W(w) | X(b) | B(b.base);
    if (rex != REX) {
        c->val[c->len++] = rex;
    }

    encode_opcode(c, enc);
    if ((enc.opextra & OPX_W) == OPX_W) {
        c->val[c->len - 1] |= w != 1;
    }

    /* If allowing sign extend of 8 bit data. */
    int s = (enc.opextra & OPX_S) == OPX_S;

    /*
     * We need to know how many bytes will be used in this instruction
     * after the address, to compute the correct relocation relative to
     * the next instruction.
     */
    w = imm_encoding_width(a, s, enc.is_displacement_or_dword);
    int d = encode_address(c, (enc.modrm >> 3) & 0x7, b, w) + 1;

    /*
     * d is number of bytes back to opcode byte to set sign extend bit.
     * Set zero if not supporting 1 byte immediate.
     */
    if (!s) {
        d = 0;
    }

    d = encode_immediate(c, a, d, enc.is_displacement_or_dword);
    assert(d == w);
}

static void encode_none(code *c, const encoding &enc, int w) {
    unsigned char rex = REX;
    if (enable_rex_w(enc.opc) && w != default_operand_size(enc.opc)) {
        rex = rex | W(w);
        if (rex != REX) {
            c->val[c->len++] = rex;
        }
    }

    encode_opcode(c, enc);
    if ((enc.opextra & OPX_W) == OPX_W) {
        c->val[c->len - 1] |= w != 1;
    }
}


static void encode_reg(code *c, encoding enc, int w, reg r, tttn cc) {
    unsigned char rex = REX | B(r);
    if (enable_rex_w(enc.opc) && w != default_operand_size(enc.opc)) {
        rex = rex | W(w);
    }

    if (rex != REX) {
        c->val[c->len++] = rex;
    }

    encode_opcode(c, enc);
    if ((enc.opextra & OPX_W) == OPX_W) {
        w = w != 1;
        if ((enc.opextra & OPX_REG) == OPX_REG) {
            w <<= 3;
        }

        c->val[c->len - 1] |= w;
    }

    if ((enc.opextra & OPX_tttn) == OPX_tttn) {
        assert(enc.opextra == OPX_tttn);
        c->val[c->len - 1] |= cc;
    }

    if ((enc.opextra & OPX_REG) == OPX_REG) {
        assert(!enc.modrm);
        c->val[c->len - 1] |= reg3(r);
    } else {
        c->val[c->len++] = 0xC0 | enc.modrm | reg3(r);
    }
}

void encode_mem(code *c, const encoding &enc, const int w, const address &addr) {
    unsigned char rex = REX | X(addr) | B(addr.base);
    if (enable_rex_w(enc.opc) && w != default_operand_size(enc.opc)) {
        rex = rex | W(w);
    }

    if (rex != REX) {
        c->val[c->len++] = rex;
    }

    encode_opcode(c, enc);
    if ((enc.opextra & OPX_W) == OPX_W) {
        c->val[c->len - 1] |= w != 1;
    }

    encode_address(c, (enc.modrm >> 3) & 0x7, addr, 0);
}

static void encode_imm(code *c, encoding enc, int w, immediate imm, tttn cc) {
    unsigned char rex = REX;
    if (enable_rex_w(enc.opc) && w != default_operand_size(enc.opc)) {
        rex = rex | W(w);
    }

    if (rex != REX) {
        c->val[c->len++] = rex;
    }

    encode_opcode(c, enc);
    if ((enc.opextra & OPX_tttn) == OPX_tttn) {
        assert(enc.opextra == OPX_tttn);
        c->val[c->len - 1] |= cc;
    }

    encode_immediate(c, imm, 1, enc.is_displacement_or_dword);
}

static int operand_size(const instruction& instr) {
    switch (instr.optype) {
        case OPT_NONE:
        case OPT_IMM:
        case OPT_REG:
        case OPT_MEM:
            return instr.source.width();
        case OPT_REG_MEM:
        case OPT_MEM_REG:
        case OPT_REG_REG:
        case OPT_IMM_REG:
        case OPT_IMM_MEM:
            if (instr.opcode == INSTR_CVTSI2S) {
                return instr.source.width();
            }
            return instr.dest.width();
        default: assert(0);
            break;
    }

    return 4;
}

int is_32_bit_imm(const immediate& imm) {
    if (imm.width <= 4) return 1;
    switch (imm.type) {
        case IMM_INT:
            return in_32bit_range(imm.qword());
        case IMM_ADDR:
            return 1; /* displacement */
        default: return 0;
    }
}


static int match_instruction_encoding(
    const instruction *instr,
    const encoding *enc)
{
    operand op{};

    assert(instr->opcode == enc->opc || enc->opc == 0);
    if (instr->optype != (enc->optype & instr->optype))
        return 0;

    switch (instr->optype) {
        case OPT_IMM:
        case OPT_IMM_REG:
        case OPT_IMM_MEM:
            if (enc->is_displacement_or_dword && !is_32_bit_imm(instr->source.imm()))
                return 0;
        default:
            break;
    }

    for (int i = 0; i < 2; ++i) {
        op = i == 0 ? instr->source : instr->dest;
        switch (enc->openc[i].implicit) {
            default: assert(0);
            case IMPL_NONE: break;
            case IMPL_AX:
                if (op.reg().r != AX) return 0;
                break;
            case IMPL_CX:
                if (op.reg().r != CX) return 0;
                break;
        }
    }

    if (enc->openc[0].widths
        && (enc->openc[0].widths & instr->source.width()) == 0)
        return 0;

    if (instr->dest.width() && enc->openc[1].widths
        && ((enc->openc[1].widths & instr->dest.width()) == 0))
        return 0;

    return 1;
}

static encoding find_encoding(const instruction &instr) {

    const auto find = [&](const encoding& enc) {
        return enc.optype == instr.optype;
    };


    auto res = encodings.begin();
    do {
        res = std::find_if(res, encodings.end(), find);
        if (res == encodings.end()) {
            assert("Unsupported instruction.");
        }
        if (match_instruction_encoding(&instr, res.base())) {
            return *res;
        }
    } while (res->opc == instr.opcode);

    assert("Unsupported instruction.");
    exit(1);
}

static int is_single_width(unsigned int w) {
    return w == 1 || w == 2 || w == 4 || w == 8 || w == 16;
}

code encode(instruction instr) {
    code c{};
    encoding enc{};


    enc = find_encoding(instr);
    for (int i = 0; i < 4 && enc.prefix[i]; ++i) {
        c.val[c.len++] = enc.prefix[i];
    }

    if (instr.prefix) {
        c.val[c.len++] = instr.prefix;
    }

    int w = operand_size(instr);
    if (w == 2) {
        c.val[c.len++] = PREFIX_OPERAND_SIZE;
    }

    switch (instr.optype) {
        default: assert(0);
        case OPT_NONE:
            encode_none(&c, enc, w);
            break;
        case OPT_REG:
            encode_reg(&c, enc, w, instr.source.reg().r, instr.cc);
            break;
        case OPT_MEM:
            encode_mem(&c, enc, w, instr.source.mem().addr);
            break;
        case OPT_IMM:
            encode_imm(&c, enc, w, instr.source.imm(), instr.cc);
            break;
        case OPT_REG_REG: {
            const int ws = instr.source.width();
            if (enc.reverse) {
                encode_reg_reg(&c, enc, ws, w, instr.dest.reg().r, instr.source.reg().r);
            } else {
                encode_reg_reg(&c, enc, ws, w, instr.source.reg().r, instr.dest.reg().r);
            }
            break;
        }
        case OPT_MEM_REG:
            encode_mem_reg(&c, enc, w, 1, instr.source.mem(), instr.dest.reg().r);
            break;
        case OPT_REG_MEM:
            encode_mem_reg(&c, enc, w, 0, instr.dest.mem(), instr.source.reg().r);
            break;
        case OPT_IMM_REG:
            encode_imm_reg(&c, enc, w, instr.source.imm(), instr.dest.reg().r);
            break;
        case OPT_IMM_MEM:
            encode_imm_mem(&c, enc, w, instr.source.imm(), instr.dest.mem().addr);
            break;
    }

    return c;
}

