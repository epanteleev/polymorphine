
#include <gtest/gtest.h>

#include "asm/Assembler.h"

TEST(Asm, ret) {
    Assembler a;
    a.ret();
    std::uint8_t v[32];
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 1);
    ASSERT_EQ(v[0], 0xC3); // 0xC3 is the opcode for RET
}

TEST(Asm, popq_reg) {
    Assembler a;

    static constexpr std::array codings = {
        0x58, // 0x58 is the opcode for POP AX
        0x59, // 0x59 is the opcode for POP CX
        0x5A, // 0x5A is the opcode for POP DX
        0x5B, // 0x5B is the opcode for POP BX
        0x5C, // 0x5C is the opcode for POP SP
        0x5D, // 0x5D is the opcode for POP BP
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

    for (const auto reg: gp_regs) {
        a.popq(reg);
    }

    std::uint8_t v[64];
    const auto size = a.to_byte_buffer(v);
    auto is_same = std::equal(codings.begin(), codings.end(), codings.begin());
    ASSERT_TRUE(is_same);
}

TEST(Asm, popq_addr) {
    Assembler a;
    Address addr(GPReg::rsp(), GPReg::noreg(), 1, 0);
    // Generate: popq (%rsp)
    a.popq(addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x04);
    ASSERT_EQ(v[2], 0x24);
}

TEST(Asm, popq_addr_with_displacement) {
    Assembler a;
    Address addr(GPReg::rsp(), GPReg::noreg(), 1, 2);
    // Generate: popq 0x1234(%rsi,%rbx,1)
    a.popq(addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x44);
    ASSERT_EQ(v[2], 0x24);
    ASSERT_EQ(v[3], 0x02);
}

TEST(Asm, popq_addr_with_index) {
    Assembler a;
    Address addr(GPReg::rsp(), GPReg::rdi(), 1, 0);
    // Generate: popq (%rsp,%rdi,1)
    a.popq(addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    unsigned char expected [] = {0x8F, 0x44, 0x3C};
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x04);
    ASSERT_EQ(v[2], 0x3C);
}

TEST(Asm, popq_addr_with_index_and_displacement) {
    Assembler a;
    Address addr(GPReg::rsp(), GPReg::rdi(), 1, 2);
    // Generate: popq 0x2(%rsp,%rdi,1)
    a.popq(addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x8F);
    ASSERT_EQ(v[1], 0x44);
    ASSERT_EQ(v[2], 0x3C);
    ASSERT_EQ(v[3], 0x02);
}

TEST(Asm, popq_addr_with_index_and_displacement2) {
    Assembler a;
    Address addr(GPReg::r15(), GPReg::rdi(), 1, 2);
    // Generate: popq 0x2(%r15,%rdi,1)
    a.popq(addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 5);
    ASSERT_EQ(v[0], 0x41);
    ASSERT_EQ(v[1], 0x8F);
    ASSERT_EQ(v[2], 0x44);
    ASSERT_EQ(v[3], 0x3f);
    ASSERT_EQ(v[4], 0x02);
}

TEST(Asm, popw_addr) {
    Assembler a;
    Address addr(GPReg::rsp(), GPReg::noreg(), 1, 0);
    // Generate: popw (%rsp)
    a.popw(addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 4);
    ASSERT_EQ(v[0], 0x66);
    ASSERT_EQ(v[1], 0x8F);
    ASSERT_EQ(v[2], 0x04);
    ASSERT_EQ(v[3], 0x24);
}

TEST(Asm, pushq_addr) {
    Assembler a;
    Address addr(GPReg::rsp(), GPReg::noreg(), 1, 0);
    // Generate: push (%rsp)
    a.pushq(addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0xFF);
    ASSERT_EQ(v[1], 0x34);
    ASSERT_EQ(v[2], 0x24);
}

TEST(Asm, movq_reg_reg) {
    Assembler a;
    a.movq(GPReg::rbx(), GPReg::rax());
    // Generate: movq %rbx, %rax
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x48);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xd8);
}

TEST(Asm, movq_reg_reg1) {
    Assembler a;
    a.movq(GPReg::r15(), GPReg::r14());
    // Generate: movq %r15, %r14
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x4d);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xfe);
}

TEST(Asm, movq_reg_reg2) {
    Assembler a;
    a.movq(GPReg::rax(), GPReg::r14());
    // Generate: movq %rax, %r14
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x49);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xc6);
}

TEST(Asm, movq_reg_reg3) {
    Assembler a;
    a.movq(GPReg::r14(), GPReg::rbx());
    // Generate: movq %r14, %rbx
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x4c);
    ASSERT_EQ(v[1], 0x89);
    ASSERT_EQ(v[2], 0xf3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}