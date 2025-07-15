
#include <cstring>
#include <iomanip>
#include <iostream>
#include <gtest/gtest.h>

#include "asm/Assembler.h"

void print_hex(const std::uint8_t* data, std::size_t size) {
    std::cout << "Hex: ";
    for (std::size_t i = 0; i < size; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << " ";
    std::cout << std::dec << std::endl;
}

class Buff final {
public:
    explicit Buff(std::span<std::uint8_t> buffer) noexcept: m_buffer(buffer) {}

    void emit8(const std::int8_t opcode) {
        m_buffer[m_size++] = opcode;
    }

    void emit16(const std::int16_t opcode) {
        std::memcpy(&m_buffer[m_size], &opcode, sizeof(opcode));
        m_size += sizeof(opcode);
    }

    void emit32(const std::int32_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 4);
        m_size += 4;
    }

    void emit64(const std::int64_t c) noexcept {
        std::memcpy(&m_buffer[m_size], &c, 8);
        m_size += 8;
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_size;
    }

private:
    std::span<std::uint8_t> m_buffer;
    std::size_t m_size{};
};

static_assert(aasm::CodeBuffer<Buff>);

std::size_t to_byte_buffer(const aasm::Assembler& aasm, std::span<std::uint8_t> buffer) {
    Buff buff{buffer};
    aasm.emit(buff);
    return buff.size();
}

TEST(Asm, ret) {
    aasm::Assembler a;
    a.ret();
    std::uint8_t v[32];
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 1);
    ASSERT_EQ(v[0], 0xC3); // 0xC3 is the opcode for RET
}

TEST(Asm, popq_reg) {
    aasm::Assembler a;

    static constexpr std::array codings = {
        0x58, // 0x58 is the opcode for POP AX
        0x59, // 0x59 is the opcode for POP CX
        0x5A, // 0x5A is the opcode for POP DX
        0x5B, // 0x5B is the opcode for POP BX
        0x5C, // 0x5C is the opcode for POP SP
        0x5D, // 0x5D is the opcode for POP BPstd::uint8_t v[32]{};
        0x5E, // 0x5E is the opcode for POP SI
        0x5F, // 0x5F is the opcode for POP DI
        0x41, 0x58, // 0x41 + 0x08 is the opcode for POP R8
        0x41, 0x59, // 0x41 + 0x09 is the opcode for POP R9
        0x41, 0x5A, // 0x41 + 0x0A is the opcode for POP R10
        0x41, 0x5B, // 0x41 + 0x0B is the opcode for POP R11
        0x41, 0x5C, // 0x41 + 0x0C is the opcode for POP R12
        0x41, 0x5D, // 0x41 + 0x0D is the opcode for POP R13
        0x41, 0x5E, // 0x41 + 0x0E is the opcode for POP R14
        0x41, 0x5F  // 0x41 + 0x0F is the opcode for POP R15
    };

    for (const auto reg: aasm::gp_regs) {
        a.pop(8, reg);
    }

    std::uint8_t v[64];
    to_byte_buffer(a, v);
    auto is_same = std::equal(codings.begin(), codings.end(), v);

    ASSERT_TRUE(is_same);
}

TEST(Asm, popq_addr) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::GPReg::noreg(), 1, 0);
    // Generate: popq (%rsp)
    a.pop(8, addr);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x04);
    ASSERT_EQ(v[2], 0x24);
}

TEST(Asm, popq_addr_with_displacement) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::GPReg::noreg(), 1, 2);
    // Generate: popq 0x1234(%rsi,%rbx,1)
    a.pop(8, addr);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x44);
    ASSERT_EQ(v[2], 0x24);
    ASSERT_EQ(v[3], 0x02);
}

TEST(Asm, popq_addr_with_index) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::rdi, 1, 0);
    // Generate: popq (%rsp,%rdi,1)
    a.pop(8, addr);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    unsigned char expected [] = {0x8F, 0x44, 0x3C};
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x04);
    ASSERT_EQ(v[2], 0x3C);
}

TEST(Asm, popq_addr_with_index_and_displacement) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::rdi, 1, 2);
    // Generate: popq 0x2(%rsp,%rdi,1)
    a.pop(8, addr);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x44);
    ASSERT_EQ(v[2], 0x3C);
    ASSERT_EQ(v[3], 0x02);
}

TEST(Asm, popq_addr_with_index_and_displacement2) {
    aasm::Assembler a;
    aasm::Address addr(aasm::r15, aasm::rdi, 1, 2);
    // Generate: popq 0x2(%r15,%rdi,1)
    a.pop(8, addr);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(v[0], 0x41);
    ASSERT_EQ(v[1], 0x8F);
    ASSERT_EQ(v[2], 0x44);
    ASSERT_EQ(v[3], 0x3f);
    ASSERT_EQ(v[4], 0x02);
}

TEST(Asm, popw_addr) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::GPReg::noreg(), 1, 0);
    // Generate: popw (%rsp)
    a.pop(2, addr);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x66);
    ASSERT_EQ(v[1], 0x8F);
    ASSERT_EQ(v[2], 0x04);
    ASSERT_EQ(v[3], 0x24);
}

TEST(Asm, pushq_addr) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::GPReg::noreg(), 1, 0);
    // Generate: push (%rsp)
    a.push(8, addr);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0xFF);
    ASSERT_EQ(v[1], 0x34);
    ASSERT_EQ(v[2], 0x24);
}

TEST(Asm, pushq_imm) {
    aasm::Assembler a;
    // Generate: push $32
    a.push(4, 320000);
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(v[0], 0x68);
    ASSERT_EQ(v[1], 0x00);
    ASSERT_EQ(v[2], 0xe2);
    ASSERT_EQ(v[3], 0x04);
    ASSERT_EQ(v[4], 0x00);
}

TEST(Asm, movq_reg_reg) {
    aasm::Assembler a;
    a.mov(8, aasm::rbx, aasm::rax);
    // Generate: movq %rbx, %rax
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x48);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xd8);
}

TEST(Asm, movq_reg_reg1) {
    aasm::Assembler a;
    a.mov(8, aasm::r15, aasm::r14);
    // Generate: movq %r15, %r14
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x4d);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xfe);
}

TEST(Asm, movq_reg_reg2) {
    aasm::Assembler a;
    a.mov(8, aasm::rax, aasm::r14);
    // Generate: movq %rax, %r14
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x49);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xc6);
}

TEST(Asm, movq_reg_reg3) {
    aasm::Assembler a;
    a.mov(8, aasm::r14, aasm::rbx);
    // Generate: movq %r14, %rbx
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x4c);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xf3);
}

TEST(Asm, movl_reg_reg3) {
    aasm::Assembler a;
    a.mov(4, aasm::r11, aasm::rdx);
    // Generate: movl %r11d, %edx
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);

    print_hex(v, 3);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x44);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xda);
}

TEST(Asm, movw_reg_reg3) {
    aasm::Assembler a;
    a.mov(2, aasm::r14, aasm::rbx);
    // Generate: movw %r14w, %bx
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x66);
    ASSERT_EQ(v[1], 0x44);
    ASSERT_EQ(v[2], 0x89);
    ASSERT_EQ(v[3], 0xf3);
}

TEST(Asm, movb_reg_reg3) {
    aasm::Assembler a;
    a.mov(1, aasm::r14, aasm::rbx);
    // Generate: movb %r14b, %bl
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x44);
    ASSERT_EQ(v[1], 0x88);
    ASSERT_EQ(v[2], 0xf3);
}

TEST(Asm, movb_imm_reg1) {
    aasm::Assembler a;
    a.mov(1, 5, aasm::rbx);
    // Generate: movb $5, %bx
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 2);
    ASSERT_EQ(v[0], 0xb3);
    ASSERT_EQ(v[1], 0x05);
}

TEST(Asm, movb_imm_reg2) {
    aasm::Assembler a;
    a.mov(1, 5, aasm::r15);
    // Generate: movb $5, %r15b
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x41);
    ASSERT_EQ(v[1], 0xb7);
    ASSERT_EQ(v[2], 0x05);
}

TEST(Asm, movw_imm_reg1) {
    aasm::Assembler a;
    a.mov(2, 256, aasm::rcx);
    // Generate: movw $256, %cx
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x66);
    ASSERT_EQ(v[1], 0xb9);
    ASSERT_EQ(v[2], 0x00);
    ASSERT_EQ(v[3], 0x01);
}

TEST(Asm, movw_imm_reg2) {
    aasm::Assembler a;
    a.mov(2, -1, aasm::r14);
    // Generate: movw $-1, %r14w
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(v[0], 0x66);
    ASSERT_EQ(v[1], 0x41);
    ASSERT_EQ(v[2], 0xbe);
    ASSERT_EQ(v[3], 0xff);
    ASSERT_EQ(v[4], 0xff);
}

TEST(Asm, movl_imm_reg1) {
    aasm::Assembler a;
    a.mov(4, -2, aasm::rsp);
    // Generate: movl $-2, %esp
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(v[0], 0xbc);
    ASSERT_EQ(v[1], 0xfe);
    ASSERT_EQ(v[2], 0xff);
    ASSERT_EQ(v[3], 0xff);
    ASSERT_EQ(v[4], 0xff);
}

TEST(Asm, movl_imm_reg2) {
    aasm::Assembler a;
    a.mov(4, 0, aasm::r12);
    // Generate: movl $0, %r12d
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, 6);
    ASSERT_EQ(v[0], 0x41);
    ASSERT_EQ(v[1], 0xbc);
    ASSERT_EQ(v[2], 0x00);
    ASSERT_EQ(v[3], 0x00);
    ASSERT_EQ(v[4], 0x00);
    ASSERT_EQ(v[5], 0x00);
}

TEST(Asm, movq_imm_reg1) {
    aasm::Assembler a;
    a.mov(8, -1, aasm::rbp);
    // Generate: movabsq $-1, %rbp
    std::uint8_t v[32]{};
    std::vector codes = {0x48,0xbd,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
        ASSERT_EQ(v[i], codes[i]) << "Mismatch at index " << i;
    }
}

TEST(Asm, movq_imm_reg2) {
    aasm::Assembler a;
    a.mov(8, 0x1234567890abcdef, aasm::r15);
    // Generate: movabsq $0x1234567890abcdef, %r15
    std::uint8_t v[32]{};
    std::vector codes = {0x49, 0xbf, 0xef, 0xcd, 0xab, 0x90, 0x78, 0x56, 0x34, 0x12};
    const auto size = to_byte_buffer(a, v);
    ASSERT_EQ(size, codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
        ASSERT_EQ(v[i], codes[i]) << "Mismatch at index " << i;
    }
}

TEST(Asm, movb_reg_mem1) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::GPReg::noreg(), 1, 2);
    a.mov(1, aasm::rbx, addr);
    // Generate: movb %bl, 2(%rsp)
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    std::vector codes = {0x88,0x5c,0x24,0x02};
    ASSERT_EQ(size, codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
        ASSERT_EQ(v[i], codes[i]) << "Mismatch at index " << i;
    }
}

TEST(Asm, movw_reg_mem1) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::rdi, 1, 2);
    a.mov(2, aasm::rsi, addr);
    // Generate: movw %si, 2(%rsp, %rdi)
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    const std::vector codes = {0x66, 0x89, 0x74, 0x3c, 0x02};
    ASSERT_EQ(size, codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
        ASSERT_EQ(v[i], codes[i]) << "Mismatch at index " << i;
    }
}

TEST(Asm, movl_reg_mem1) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::rdi, 8, INT8_MAX);
    a.mov(4, aasm::rdi, addr);
    // Generate: movl %edi, 127(%rsp, %rdi, 8)
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    const std::vector codes = {0x89, 0x7c, 0xfc, 0x7f};
    ASSERT_EQ(size, codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
        ASSERT_EQ(v[i], codes[i]) << "Mismatch at index " << i;
    }
}

TEST(Asm, movq_reg_mem1) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsi, aasm::rdi, 8, INT32_MAX);
    a.mov(8, aasm::rdi, addr);
    // Generate: movq %rdi, 2147483647(%rsi, %rdi, 8)
    std::uint8_t v[32]{};
    const auto size = to_byte_buffer(a, v);
    const std::vector codes = {0x48, 0x89, 0xbc, 0xfe, 0xff, 0xff, 0xff, 0x7f};
    ASSERT_EQ(size, codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
        ASSERT_EQ(v[i], codes[i]) << "Mismatch at index " << i;
    }
}

static std::string make_string(const aasm::Assembler &a) {
    std::ostringstream os;
    os << a;
    return os.str();
}


static void check_bytes(const std::vector<std::vector<std::uint8_t>>& codes, const std::vector<std::string>& names, aasm::Assembler(*fn)(std::uint8_t)) {
    ASSERT_EQ(codes.size(), names.size());
    ASSERT_GT(codes.size(), 0U) << "No codes provided for testing";

    for (auto scale : std::views::iota(0U, codes.size())) {
        aasm::Assembler a = fn(1 << scale);
        std::uint8_t v[aasm::constants::MAX_X86_INSTRUCTION_SIZE]{};
        const auto size = to_byte_buffer(a, v);
        auto& code = codes[scale];

        ASSERT_EQ(size, code.size()) << "Mismatch at scale=" << scale;
        for (std::size_t i = 0; i < code.size(); ++i) {
            ASSERT_EQ(v[i], code[i]) << "Mismatch at index=" << i << " scale=" << scale;
        }

        auto& name = names[scale];
        ASSERT_EQ(name, make_string(a)) << "Mismatch at scale=" << scale;
    }
}

TEST(Asm, mov_mem_reg2) {
    std::vector<std::vector<std::uint8_t>> codes = {
        {0x40, 0x8a, 0x36},
        {0x66,0x8b,0x36},
        {0x8b,0x36},
        {0x48,0x8b,0x36}
    };

    std::vector<std::string> names = {
        "movb (%rsi), %sil",
        "movw (%rsi), %si",
        "movl (%rsi), %esi",
        "movq (%rsi), %rsi",
    };

    const auto generator = [](const std::uint8_t size) {
        aasm::Assembler a;
        aasm::Address addr(aasm::rsi, aasm::GPReg::noreg(), 1);
        a.mov(size, addr, aasm::rsi);
        return a;
    };

    check_bytes(codes, names, generator);
}

TEST(Asm, mov_mem_imm1) {
    std::vector<std::vector<std::uint8_t>> codes = {
        {0xc6,0x06,0x12},
        {0x66,0xc7,0x06,0x12,0x00},
        {0xc7,0x06,0x12,0x00,0x00,0x00},
        {0x48,0xc7,0x06,0x12,0x00,0x00,0x00}
    };
    std::vector<std::string> names = {
        "movb $18, (%rsi)",
        "movw $18, (%rsi)",
        "movl $18, (%rsi)",
        "movq $18, (%rsi)"
    };

    const auto generator = [](const std::uint8_t size) {
        aasm::Assembler a;
        aasm::Address addr(aasm::rsi, aasm::GPReg::noreg(), 1);
        a.mov(size, 0x12, addr);
        return a;
    };

    check_bytes(codes, names, generator);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}