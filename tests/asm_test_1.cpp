#include <gtest/gtest.h>

#include "Buff.h"

TEST(Asm1, forward_jmp1) {
    aasm::Assembler a;
    const auto label = a.create_label();
    a.jmp(label);
    a.mov(8, 8, aasm::rax);
    a.set_label(label);
    a.ret();

    /*
     jmp L1
     movabsq $8, %rax
     L1:
     ret
     */

    std::uint8_t v[64];
    const auto size = to_byte_buffer(a, v);
    print_hex(reinterpret_cast<const std::uint8_t *>(&v), size);
   // ASSERT_EQ(size, 1);
   // ASSERT_EQ(v[0], 0xC3); // 0xC3 is the opcode for RET
}

TEST(Asm1, backward_jmp1) {
    aasm::Assembler a;
    const auto label = a.create_label();
    a.set_label(label);
    a.mov(8, 8, aasm::rax);
    a.jmp(label);
    a.ret();

    /*
     L1:
     movabsq $8, %rax
     jmp L1
     ret
     */

    std::vector<std::uint8_t> codes = {
        0x48,0xb8,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0xEB, 0xf4,
        0xC3
    };
    std::uint8_t v[64];
    const auto size = to_byte_buffer(a, v);
    print_hex(reinterpret_cast<const std::uint8_t *>(&v), size);
    for (std::size_t i = 0; i < size; ++i) {
        ASSERT_EQ(codes[i], v[i]) << "codes[" << i << "] = " << codes[i];
    }
    ASSERT_EQ(size, 13);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}