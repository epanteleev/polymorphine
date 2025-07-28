#include <gtest/gtest.h>

#include "helpers/Utils.h"

static void verify_codes(const std::vector<std::uint8_t>& codes, const std::uint8_t* v) {
    for (std::size_t i = 0; i < codes.size(); ++i) {
        ASSERT_EQ(codes[i], v[i]) << "codes[" << i << "] = " << codes[i];
    }
}

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

    std::vector<std::uint8_t> codes = {
        0xe9, 0x0a, 0x00, 0x00, 0x00,
        0x48, 0xb8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xc3
    };

    std::uint8_t v[64];
    const auto size = to_byte_buffer(a.to_buffer(), v);
    print_hex(reinterpret_cast<const std::uint8_t *>(&v), size);
    ASSERT_EQ(size, codes.size());
    verify_codes(codes, v);
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
        0xEB,0xf4,
        0xC3
    };
    std::uint8_t v[64];
    const auto size = to_byte_buffer(a.to_buffer(), v);
    print_hex(reinterpret_cast<const std::uint8_t *>(&v), size);
    verify_codes(codes, v);
    ASSERT_EQ(size, 13);
}

TEST(Asm1, forward_cond_jmp1) {
    aasm::Assembler a;
    const auto label = a.create_label();
    a.cmp(8, 8, aasm::rax);
    a.jcc(aasm::CondType::A, label);
    a.mov(8, 8, aasm::rax);
    a.set_label(label);
    a.ret();

    /*
     cmpq $8, %rax
     ja L1
     movabsq $8, %rax
     L1:
     ret
     */

    std::vector<std::uint8_t> codes = {
        0x48, 0x83, 0xf8, 0x08,
        0x0F, 0x87, 0x0A, 0x00, 0x00, 0x00,
        0x48, 0xb8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xC3
    };
    std::uint8_t v[64];
    const auto size = to_byte_buffer(a.to_buffer(), v);
    print_hex(reinterpret_cast<const std::uint8_t *>(&v), size);
    ASSERT_EQ(size, codes.size());
    verify_codes(codes, v);
}

TEST(Asm1, backward_cond_jmp1) {
    aasm::Assembler a;
    const auto label = a.create_label();
    a.set_label(label);
    a.mov(8, 8, aasm::rax);
    a.cmp(8, 8, aasm::rax);
    a.jcc(aasm::CondType::A, label);
    a.ret();

    /*
     L1:
     movabsq $8, %rax
     cmpq $8, %rax
     ja L1
     ret
     */

    std::vector<std::uint8_t> codes = {
        0x48, 0xb8, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x48, 0x83, 0xf8, 0x08,
        0x77, 0xf0,
        0xC3
    };
    std::uint8_t v[64];
    const auto size = to_byte_buffer(a.to_buffer(), v);
    print_hex(reinterpret_cast<const std::uint8_t *>(&v), size);
    ASSERT_EQ(size, codes.size());
    verify_codes(codes, v);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}