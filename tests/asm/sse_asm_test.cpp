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

TEST(SSE_Asm, movss_reg_addr) {
    const std::vector<std::uint8_t> codes = {0xf3,0x0f,0x11,0x4d,0x03};

    aasm::AsmEmitter a;
    constexpr aasm::Address addr(aasm::rbp, 3);
    a.movss(aasm::xmm1, addr);
    check_coding(std::move(a), codes, "movss %xmm1, 3(%rbp)");
}

TEST(SSE_Asm, movsd_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf2,0x0f,0x10,0xd1};

    aasm::AsmEmitter a;
    a.movsd(aasm::xmm1, aasm::xmm2);
    check_coding(std::move(a), codes, "movsd %xmm1, %xmm2");
}

TEST(SSE_Asm, movsd_reg_reg_high) {
    const std::vector<std::uint8_t> codes = {0xf2,0x44,0x0f,0x10,0xd1};

    aasm::AsmEmitter a;
    a.movsd(aasm::xmm1, aasm::xmm10);
    check_coding(std::move(a), codes, "movsd %xmm1, %xmm10");
}

TEST(SSE_Asm, movsd_addr_reg) {
    const std::vector<std::uint8_t> codes = {0xf2,0x0f,0x10,0x4d,0x03};

    aasm::AsmEmitter a;
    constexpr aasm::Address addr(aasm::rbp, 3);
    a.movsd(addr, aasm::xmm1);
    check_coding(std::move(a), codes, "movsd 3(%rbp), %xmm1");
}

TEST(SSE_Asm, movsd_reg_addr) {
    const std::vector<std::uint8_t> codes = {0xf2,0x0f,0x11,0x4d,0x03};

    aasm::AsmEmitter a;
    constexpr aasm::Address addr(aasm::rbp, 3);
    a.movsd(aasm::xmm1, addr);
    check_coding(std::move(a), codes, "movsd %xmm1, 3(%rbp)");
}

TEST(SSE_Asm, movsd_addr_reg_high) {
    const std::vector<std::uint8_t> codes = {0xf2,0x44,0x0f,0x10,0x4d,0x03};

    aasm::AsmEmitter a;
    constexpr aasm::Address addr(aasm::rbp, 3);
    a.movsd(addr, aasm::xmm9);
    check_coding(std::move(a), codes, "movsd 3(%rbp), %xmm9");
}

TEST(SSE_Asm, ucomisd_reg_reg) {
    const std::vector<std::uint8_t> codes = {0x66,0x0f,0x2e,0xd1};

    aasm::AsmEmitter a;
    a.ucomisd(aasm::xmm1, aasm::xmm2);
    check_coding(std::move(a), codes, "ucomisd %xmm1, %xmm2");
}

TEST(SSE_Asm, comisd_reg_reg) {
    const std::vector<std::uint8_t> codes = {0x66,0x0f,0x2f,0xd1};

    aasm::AsmEmitter a;
    a.comisd(aasm::xmm1, aasm::xmm2);
    check_coding(std::move(a), codes, "comisd %xmm1, %xmm2");
}

TEST(SSE_Asm, comiss_reg_reg) {
    const std::vector<std::uint8_t> codes = {0x0f,0x2f,0xd1};

    aasm::AsmEmitter a;
    a.comiss(aasm::xmm1, aasm::xmm2);
    check_coding(std::move(a), codes, "comiss %xmm1, %xmm2");
}

TEST(SSE_Asm, cvttss2siq_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf3,0x48,0x0f,0x2c,0xc1};

    aasm::AsmEmitter a;
    a.cvtss2si(8, aasm::xmm1, aasm::rax);
    check_coding(std::move(a), codes, "cvttss2siq %xmm1, %rax");
}

TEST(SSE_Asm, cvttss2sil_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf3,0x0f,0x2c,0xc1};

    aasm::AsmEmitter a;
    a.cvtss2si(4, aasm::xmm1, aasm::rax);
    check_coding(std::move(a), codes, "cvttss2sil %xmm1, %eax");
}

TEST(SSE_Asm, cvttsd2siq_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf2,0x48,0x0f,0x2c,0xc1};

    aasm::AsmEmitter a;
    a.cvtsd2si(8, aasm::xmm1, aasm::rax);
    check_coding(std::move(a), codes, "cvttsd2siq %xmm1, %rax");
}

TEST(SSE_Asm, cvttsd2sil_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf2,0x0f,0x2c,0xc1};

    aasm::AsmEmitter a;
    a.cvtsd2si(4, aasm::xmm1, aasm::rax);
    check_coding(std::move(a), codes, "cvttsd2sil %xmm1, %eax");
}

TEST(SSE_Asm, cvtsi2ssq_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf3,0x48,0x0f,0x2a,0xc8};

    aasm::AsmEmitter a;
    a.cvtsi2ss(8, aasm::rax, aasm::xmm1);
    check_coding(std::move(a), codes, "cvtsi2ss %rax, %xmm1");
}

TEST(SSE_Asm, cvtsi2ssl_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf3,0x0f,0x2a,0xc8};

    aasm::AsmEmitter a;
    a.cvtsi2ss(4, aasm::rax, aasm::xmm1);
    check_coding(std::move(a), codes, "cvtsi2ss %eax, %xmm1");
}

TEST(SSE_Asm, cvtsi2sdq_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf2,0x48,0x0f,0x2a,0xc8};

    aasm::AsmEmitter a;
    a.cvtsi2sd(8, aasm::rax, aasm::xmm1);
    check_coding(std::move(a), codes, "cvtsi2sd %rax, %xmm1");
}

TEST(SSE_Asm, cvtsi2sdl_reg_reg) {
    const std::vector<std::uint8_t> codes = {0xf2,0x0f,0x2a,0xc8};

    aasm::AsmEmitter a;
    a.cvtsi2sd(4, aasm::rax, aasm::xmm1);
    check_coding(std::move(a), codes, "cvtsi2sd %eax, %xmm1");
}

TEST(SSE_Asm, cvtsi2sdq_reg_reg1) {
    const std::vector<std::uint8_t> codes = {0xf2,0x41,0x0f,0x2a,0xc3};

    aasm::AsmEmitter a;
    a.cvtsi2sd(4, aasm::r11, aasm::xmm0);
    check_coding(std::move(a), codes, "cvtsi2sd %r11d, %xmm0");
}

TEST(SSE_Asm, cvtsi2sdq_adr_reg1) {
    const std::vector<std::uint8_t> codes = {0xf2,0x0f,0x2a,0x06};

    aasm::AsmEmitter a;
    constexpr aasm::Address adr(aasm::rsi);
    a.cvtsi2sd(4, adr, aasm::xmm0);
    check_coding(std::move(a), codes, "cvtsi2sd (%rsi), %xmm0");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}