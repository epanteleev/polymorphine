#include <gtest/gtest.h>

#include "helpers/Utils.h"
#include "asm/x64/asm.h"


TEST(SSE_Asm, movss_reg_reg) {
    const std::vector<std::vector<std::uint8_t>> codes = {
        {0xf3,0x0f,0x10,0xd1},
    };
    const std::vector<std::string> names = {
        "movss %xmm1, %xmm2",
    };

    const auto generator = [](const std::uint8_t) {
        aasm::AsmEmitter a;
        a.movss(aasm::xmm1, aasm::xmm2);
        return a;
    };

    check_bytes(codes, names, generator, std::array{4});
}

TEST(SSE_Asm, movss_reg_reg_high) {
    const std::vector<std::vector<std::uint8_t>> codes = {
        {0xf3,0x44,0x0f,0x10,0xd1},
    };
    const std::vector<std::string> names = {
        "movss %xmm1, %xmm10",
    };

    const auto generator = [](const std::uint8_t) {
        aasm::AsmEmitter a;
        a.movss(aasm::xmm1, aasm::xmm10);
        return a;
    };

    check_bytes(codes, names, generator, std::array{4});
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}