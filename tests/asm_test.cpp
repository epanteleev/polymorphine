
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

TEST(Asm, ret) {
    aasm::Assembler a;
    a.ret();
    std::uint8_t v[32];
    const auto size = a.to_byte_buffer(v);
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

    for (const auto reg: aasm::gp_regs) {
        a.pop(8, reg);
    }

    std::uint8_t v[64];
    const auto size = a.to_byte_buffer(v);
    auto is_same = std::equal(codings.begin(), codings.end(), codings.begin());
    ASSERT_TRUE(is_same);
}

TEST(Asm, popq_addr) {
    aasm::Assembler a;
    aasm::Address addr(aasm::rsp, aasm::GPReg::noreg(), 1, 0);
    // Generate: popq (%rsp)
    a.pop(8, addr);
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0xFF);
    ASSERT_EQ(v[1], 0x34);
    ASSERT_EQ(v[2], 0x24);
}

TEST(Asm, movq_reg_reg) {
    aasm::Assembler a;
    a.mov(8, aasm::rbx, aasm::rax);
    // Generate: movq %rbx, %rax
    std::uint8_t v[32]{};
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);

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
    const auto size = a.to_byte_buffer(v);
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
    const auto size = a.to_byte_buffer(v);
    ASSERT_EQ(size, 3);
    ASSERT_EQ(v[0], 0x44);
    ASSERT_EQ(v[1], 0x88);
    ASSERT_EQ(v[2], 0xf3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}