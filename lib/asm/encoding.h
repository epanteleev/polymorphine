#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <variant>

enum reg: std::uint8_t {
    AX  = 1,    /* 0b000 */
    CX  = 2,    /* 0b001 */
    DX  = 3,    /* 0b010 */
    BX  = 4,    /* 0b011 */
    SP  = 5,    /* 0b100 */
    BP  = 6,    /* 0b101 */
    SI  = 7,    /* 0b110 */
    DI  = 8,    /* 0b111 */
    R8  = 9,
    R9  = 10,
    R10 = 11,
    R11 = 12,
    R12 = 13,
    R13 = 14,
    R14 = 15,
    R15 = 16,

    /*
     * Floating point registers. REX prefix needed for XMM8 through 15,
     * which are index 24 (11000) through 31 (0b11111).
     */
    XMM0, XMM1,  XMM2,  XMM3,  XMM4,  XMM5,  XMM6,  XMM7,
    XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,

    /* Instruction pointer. */
    IP,

    /* X87 extended precision floating point registers. */
    ST0,
    ST1,
    ST2,
    ST3,
    ST4,
    ST5,
    ST6,
    ST7
};

/* Register and width, which is 1, 2, 4 or 8. */
struct registr {
    int width;
    reg r;
};


enum AddrType: std::uint8_t {
    ADDR_NORMAL,
    ADDR_GLOBAL_OFFSET,
    ADDR_PLT
};

struct address {
    explicit constexpr address(reg base, reg index = {}, unsigned scale = 1, int displacement = 0): type(ADDR_NORMAL),
        displacement(displacement),
        base(base),
        index(index),
        scale(scale) {}

    AddrType type;

    //const struct symbol *sym{};
    int displacement;

    reg base;
    reg index;
    unsigned scale;
};

/* Memory location; address and width. */
struct memory {
    int width;
    address addr;
};


enum ImmType: std::uint8_t {
    IMM_INT,    /* 1, 2, 4 or 8 byte signed number. */
    IMM_ADDR,   /* Symbol-relative address, label etc. */
    IMM_STRING  /* string value, only used for initialization. */
};

/*
 * Immediates can be numeric (fit in registers), or references to static
 * string values. Expressions like "hello" + 1 can result in for ex
 * .LC1+1 in GNU assembly.
 */
struct immediate {

    [[nodiscard]]
    const long& qword() const {
        return std::get<long>(d);
    }

    [[nodiscard]]
    const int& dword() const {
        return std::get<int>(d);
    }

    [[nodiscard]]
    const short& word() const {
        return std::get<short>(d);
    }

    [[nodiscard]]
    const char& byte() const {
        return std::get<char>(d);
    }

    [[nodiscard]]
    const address& addr() const {
        return std::get<address>(d);
    }

    int width;
    ImmType type;
    std::variant<char, short, int, long, address, std::string> d;
};


/*
 * Opcodes map to first entry in encoding table.
 */
enum opcodes: std::uint8_t {
    INSTR_ADD = 0,
    INSTR_AND = INSTR_ADD + 2,
    INSTR_CALL = INSTR_AND + 3,
    INSTR_CMP = INSTR_CALL + 2,
    INSTR_Cxy = INSTR_CMP + 3,          /* Sign extend %[e/r]ax to %[e|r]dx:%[e|r]ax. */
    INSTR_DIV = INSTR_Cxy + 2,
    INSTR_IDIV = INSTR_DIV + 1,         /* Signed division. */
    INSTR_Jcc = INSTR_IDIV + 1,         /* Jump on condition (combined with tttn) */
    INSTR_JMP = INSTR_Jcc + 1,
    INSTR_LEA = INSTR_JMP + 1,
    INSTR_LEAVE = INSTR_LEA + 1,
    INSTR_MOV = INSTR_LEAVE + 1,
    INSTR_MOV_STR = INSTR_MOV + 5,      /* Move string, optionally with REP prefix. */
    INSTR_MOVSX = INSTR_MOV_STR + 1,
    INSTR_MOVZX = INSTR_MOVSX + 2,
    INSTR_MUL = INSTR_MOVZX + 2,
    INSTR_NOT = INSTR_MUL + 1,
    INSTR_OR = INSTR_NOT + 1,
    INSTR_POP = INSTR_OR + 2,
    INSTR_PUSH = INSTR_POP + 1,
    INSTR_RET = INSTR_PUSH + 3,
    INSTR_SAR = INSTR_RET + 1,
    INSTR_SETcc = INSTR_SAR + 2,        /* Set flag (combined with tttn). */
    INSTR_SHL = INSTR_SETcc + 1,
    INSTR_SHR = INSTR_SHL + 2,
    INSTR_SUB = INSTR_SHR + 2,
    INSTR_TEST = INSTR_SUB + 2,
    INSTR_XOR = INSTR_TEST + 2,

    INSTR_ADDS = INSTR_XOR + 2,         /* Add floating point. */
    INSTR_CVTSI2S = INSTR_ADDS + 6,     /* Convert int to floating point. */
    INSTR_CVTS2S = INSTR_CVTSI2S + 2,   /* Convert between float and double. */
    INSTR_CVTTS2SI = INSTR_CVTS2S + 2,  /* Convert floating point to int with truncation. */
    INSTR_DIVS = INSTR_CVTTS2SI + 2,
    INSTR_MULS = INSTR_DIVS + 2,        /* Multiply floating point. */
    INSTR_SUBS = INSTR_MULS + 3,        /* Subtract floating point. */
    INSTR_MOVAP = INSTR_SUBS + 2,       /* Move aligned packed floating point. */
    INSTR_MOVS = INSTR_MOVAP + 2,       /* Move floating point. */
    INSTR_UCOMIS = INSTR_MOVS + 6,      /* Compare floating point and set EFLAGS. */
    INSTR_PXOR = INSTR_UCOMIS + 2,      /* Bitwise xor with xmm register. */

    INSTR_FADDP = INSTR_PXOR + 1,       /* Add x87 ST(0) to ST(i) and pop. */
    INSTR_FDIVRP = INSTR_FADDP + 1,     /* Divide and pop. */
    INSTR_FILD = INSTR_FDIVRP + 1,      /* Load integer to ST(0). */
    INSTR_FISTP = INSTR_FILD + 3,       /* Store integer and pop. */
    INSTR_FLD = INSTR_FISTP + 3,        /* Load x87 real to ST(0). */
    INSTR_FLDZ = INSTR_FLD + 4,         /* Push +0.0. */
    INSTR_FLDCW = INSTR_FLDZ + 1,       /* Load x87 FPU control word. */
    INSTR_FMULP = INSTR_FLDCW + 1,      /* Multiply and pop. */
    INSTR_FNSTCW = INSTR_FMULP + 1,     /* Store x87 FPU control word. */
    INSTR_FSTP = INSTR_FNSTCW + 1,      /* Store x87 real from ST(0) to mem and pop. */
    INSTR_FSUBRP = INSTR_FSTP + 4,      /* Subtract and pop. */
    INSTR_FUCOMIP = INSTR_FSUBRP + 1,   /* Compare x87 floating point. */
    INSTR_FXCH = INSTR_FUCOMIP + 1      /* Swap ST(0) with ST(i). */
};

enum prefixes: std::uint8_t {
    PREFIX_NONE = 0x0,
    PREFIX_REP = 0xF3,
    PREFIX_REPE = 0xF3,
    PREFIX_REPNE = 0xF2
};

/*
 * Conditional test/jump codes. A nice reference is
 * http://unixwiz.net/techtips/x86-jumps.html
 */
enum tttn: std::uint8_t {
    CC_O = 0x0,
    CC_NO = 0x1,
    CC_NAE = 0x2,
    CC_AE = 0x3,
    CC_E = 0x4,
    CC_NE = 0x5,
    CC_NA = 0x6,
    CC_A = 0x7,
    CC_S = 0x8,
    CC_NS = 0x9,
    CC_P = 0xA,
    CC_NP = 0xB,
    CC_NGE = 0xC,
    CC_GE = 0xD,
    CC_NG = 0xE,
    CC_G = 0xF
};


enum instr_optype: std::uint8_t {
    OPT_NONE = 0,
    OPT_IMM = 1,
    OPT_REG = 2,
    OPT_MEM = 4,
    OPT_REG_REG = 8,
    OPT_REG_MEM = 16,
    OPT_MEM_REG = 32,
    OPT_IMM_REG = 64,
    OPT_IMM_MEM = 128
};

struct operand final {
    [[nodiscard]]
    const int& width() const {
        return std::get<int>(m_data);
    }

    [[nodiscard]]
    const registr& reg() const {
        return std::get<registr>(m_data);
    }

    [[nodiscard]]
    const memory& mem() const {
        return std::get<memory>(m_data);
    }

    [[nodiscard]]
    const immediate& imm() const {
        return std::get<immediate>(m_data);
    }

    std::variant<
        int, /* Width of operand, 1, 2, 4 or 8. */
        registr, /* Register operand. */
        memory, /* Memory operand. */
        immediate /* Immediate operand. */
    > m_data;
};

/* Instructions with register, memory or immediate operands. */
struct instruction {
    opcodes opcode;
    tttn cc;
    prefixes prefix;
    instr_optype optype;
    operand source;
    operand dest;
};

/*
 * According to Intel reference manual, instructions can contain the
 * following fields:
 *
 *  [Legacy Prefixes] [REX] [Opcode] [ModR/M] [SIB] [Displ] [Immediate]
 *   (up to 4 bytes)   (1)    (3)      (1)     (1)    (4)       (4)
 *
 *
 * At most 15 bytes can be used for one instruction.
 */
struct code {
    code() = default;
    explicit code(const std::uint8_t c): len(1) {
        val[0] = c;
    }

    void emit8(std::uint8_t c) noexcept {
        val[len++] = c;
    }

    unsigned char val[15]{};
    std::uint8_t len{};
};

static constexpr auto PREFIX_OPERAND_SIZE = 0x66;

static constexpr std::uint8_t reg3(reg arg) noexcept {
    return arg - 1 & 0x7;
}

static constexpr bool is_64_bit_reg(const std::size_t arg) noexcept {
    return (arg >= static_cast<std::size_t>(reg::R8) && arg <= static_cast<std::size_t>(reg::R15)) ||
           (arg >= static_cast<std::size_t>(reg::XMM8) && arg <= static_cast<std::size_t>(reg::XMM15));
}

template<typename T>
static constexpr bool in_byte_range(T arg) {
    return arg >= -128 && arg <= 127;
}

template<typename T>
static bool in_32bit_range(T arg) noexcept {
    return arg >= -2147483648 && arg <= 2147483647;
}

struct encoding {
    opcodes opc;

    struct {
        const char *str;
        unsigned int suffix : 1;
    } mnemonic;

    /*
     * Prefixes are separated into four groups, and there can be only
     * one prefix from each group, in any order.
     *
     *   Group 1: 0xF0, 0xF2, 0xF3
     *   Group 2: 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x2E, 0x3E
     *   Group 3: 0x66 (Operand size override, set 16 bit)
     *   Group 4: 0x67 (Address size override)
     *
     */
    unsigned char prefix[4];

    /* There can be up to 3 opcode bytes. */
    unsigned char opcode[3];

    /* Optionally extra stuff in opcode byte. */
    unsigned char opextra;

    /* Base pattern for ModR/M byte. */
    unsigned char modrm;

    unsigned char optype;

    /* Restrictions and encoding information for each operand. */
    struct {
        unsigned int widths : 4;
        unsigned int implicit : 4;
    } openc[2];

    /* Some instructions have their operands reversed. */
    unsigned int reverse : 1;

    /*
     * Mark that immediate operand is a relative displacement, and not a
     * direct address. Used for example in jump instructions.
     *
     * Also used to restrict encoding of integer constants that can fit
     * in 32 bit, for instructions that implicitly extend to 64 bit.
     */
    unsigned int is_displacement_or_dword : 1;
};

/*
 * Some additional information can be encoded in the last opcode byte:
 *
 *   w: Set to 1 if operand size should be full size (32 or 64),
 *      otherwise operand size is 1 byte. Normally least significant bit
 *      of last opcode byte, unless OPX_REG is set, in which case it is
 *      the 4th bit (after reg).
 *
 *   s: Set to 1 to sign extend immediate into operand size. Bit 2 of
 *      last opcode byte.
 *
 *   d: Direction bit, reverse mem/reg operands. Bit 2 of last opcode
 *      byte.
 *
 */
enum opextra: std::uint8_t {
    OPX_NONE = 0,
    OPX_W = 1,
    OPX_S = 2,
    OPX_SW = OPX_S | OPX_W,
    OPX_D = 4,
    OPX_DW = OPX_D | OPX_W,
    OPX_tttn = 8,
    OPX_REG = 16,
    OPX_WREG = OPX_W | OPX_REG
};

template<typename T>
static constexpr T W(T arg) noexcept {
    return (arg == 8) << 3;
}

template<typename T>
static constexpr std::uint8_t R(T arg) noexcept {
    return static_cast<T>(is_64_bit_reg(arg)) << 2;
}

static constexpr std::uint8_t X(const address& arg) {
    return is_64_bit_reg(arg.index) << 1;
}

static constexpr std::uint8_t B(reg arg) {
    return is_64_bit_reg(arg);
}

/* Hack to enable REX.W on certain SSE instructions. */
#define is_general(op) (op <= INSTR_XOR)
#define enable_rex_w(op) (is_general(op) || op == INSTR_CVTTS2SI || op == INSTR_CVTSI2S)


void encode_mem(code *c, const encoding &enc, int w, const address &addr);

code encode(instruction instr);