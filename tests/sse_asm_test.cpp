#include <gtest/gtest.h>

#include "helpers/Utils.h"
#include "asm/x64/asm.h"


TEST(SSE_Asm, movss_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf3,0x0f,0x10,0xd1};

    aasm::AsmEmitter a;
    a.movss(aasm::xmm1, aasm::xmm2);
    check_coding(std::move(a), codes, "movss %xmm1, %xmm2");
}

TEST(SSE_Asm, movss_reg_reg_high) {
    const std::vector<std::uint8_t> codes = {0xf3,0x44,0x0f,0x10,0xd1};

    aasm::AsmEmitter a;
    a.movss(aasm::xmm1, aasm::xmm10);
    check_coding(std::move(a), codes, "movss %xmm1, %xmm10");
}

TEST(SSE_Asm, movss_addr_reg) {
    const std::vector<std::uint8_t> codes = {0xf3,0x0f,0x10,0x4d,0x03};

    aasm::AsmEmitter a;
    constexpr aasm::Address addr(aasm::rbp, 3);
    a.movss(addr, aasm::xmm1);
    check_coding(std::move(a), codes, "movss 3(%rbp), %xmm1");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}