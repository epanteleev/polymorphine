#include <gtest/gtest.h>

#include "helpers/Utils.h"

static std::vector cond_types = {
    aasm::CondType::O,
    aasm::CondType::NO,
    aasm::CondType::NAE,
    aasm::CondType::AE,
    aasm::CondType::E,
    aasm::CondType::NE,
    aasm::CondType::NA,
    aasm::CondType::A,
    aasm::CondType::S,
    aasm::CondType::NS,
    aasm::CondType::P,
    aasm::CondType::NP,
    aasm::CondType::NGE,
    aasm::CondType::GE,
    aasm::CondType::NG,
    aasm::CondType::G
};

TEST(Asm, cmov_reg_reg) {
    const std::vector<std::string> names = {
        "cmovo %r12w, %r13w",
        "cmovno %r12w, %r13w",
        "cmovnae %r12w, %r13w",
        "cmovae %r12w, %r13w",
        "cmove %r12w, %r13w",
        "cmovne %r12w, %r13w",
        "cmovna %r12w, %r13w",
        "cmova %r12w, %r13w",
        "cmovs %r12w, %r13w",
        "cmovns %r12w, %r13w",
        "cmovp %r12w, %r13w",
        "cmovnp %r12w, %r13w",
        "cmovnge %r12w, %r13w",
        "cmovge %r12w, %r13w",
        "cmovng %r12w, %r13w",
        "cmovg %r12w, %r13w"
    };
    std::vector<std::vector<std::uint8_t>> codes = {
        {0x66,0x45,0x0f,0x40,0xEC}, // cmovo %r12w, %r13w
        {0x66,0x45,0x0f,0x41,0xEC}, // cmovno %r12w, %r13w
        {0x66,0x45,0x0f,0x42,0xEC}, // cmovnae %r12w, %r13w
        {0x66,0x45,0x0f,0x43,0xEC}, // cmovae %r12w, %r13w
        {0x66,0x45,0x0f,0x44,0xEC}, // cmove %r12w, %r13w
        {0x66,0x45,0x0f,0x45,0xEC}, // cmovne %r12w, %r13w
        {0x66,0x45,0x0f,0x46,0xEC}, // cmovna %r12w, %r13w
        {0x66,0x45,0x0f,0x47,0xEC}, // cmova %r12w, %r13w
        {0x66,0x45,0x0f,0x48,0xEC}, // cmovs %r12w, %r13w
        {0x66,0x45,0x0f,0x49,0xEC}, // cmovns %r12w, %r13w
        {0x66,0x45,0x0f,0x4A,0xEC}, // cmovp %r12w, %r13w
        {0x66,0x45,0x0f,0x4B,0xEC}, // cmovnp %r12w, %r13w
        {0x66,0x45,0x0f,0x4C,0xEC}, // cmovnge %r12w, %r13w
        {0x66,0x45,0x0f,0x4D,0xEC}, // cmovge %r12w, %r13w
        {0x66,0x45,0x0f,0x4E,0xEC}, // cmovng %r12w, %r13w
        {0x66,0x45,0x0f,0x4F,0xEC}  // cmovg %r12w, %r
    };

    const auto generator = [](aasm::CondType type) {
        aasm::AsmEmitter a;
        a.cmov(2, type, aasm::r12, aasm::r13);
        return a;
    };

    check_bytes_cc(codes, names, generator, cond_types);
}

TEST(Asm, cmov_reg_mem) {
    std::vector<std::string> names = {
        "cmovo 8(%r12,%r13,4), %r12",
        "cmovno 8(%r12,%r13,4), %r12",
        "cmovnae 8(%r12,%r13,4), %r12",
        "cmovae 8(%r12,%r13,4), %r12",
        "cmove 8(%r12,%r13,4), %r12",
        "cmovne 8(%r12,%r13,4), %r12",
        "cmovna 8(%r12,%r13,4), %r12",
        "cmova 8(%r12,%r13,4), %r12",
        "cmovs 8(%r12,%r13,4), %r12",
        "cmovns 8(%r12,%r13,4), %r12",
        "cmovp 8(%r12,%r13,4), %r12",
        "cmovnp 8(%r12,%r13,4), %r12",
        "cmovnge 8(%r12,%r13,4), %r12",
        "cmovge 8(%r12,%r13,4), %r12",
        "cmovng 8(%r12,%r13,4), %r12",
        "cmovg 8(%r12,%r13,4), %r12"
    };

    std::vector<std::vector<std::uint8_t>> codes = {
        {0x4f,0x0f,0x40,0x64,0xac,0x08}, // cmovo 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x41,0x64,0xac,0x08}, // cmovno 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x42,0x64,0xac,0x08}, // cmovnae 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x43,0x64,0xac,0x08}, // cmovae 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x44,0x64,0xac,0x08}, // cmove 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x45,0x64,0xac,0x08}, // cmovne 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x46,0x64,0xac,0x08}, // cmovna 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x47,0x64,0xac,0x08}, // cmova 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x48,0x64,0xac,0x08}, // cmovs 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x49,0x64,0xac,0x08}, // cmovns 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x4A,0x64,0xac,0x08}, // cmovp 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x4B,0x64,0xac,0x08}, // cmovnp 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x4C,0x64,0xac,0x08}, // cmovnge 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x4D,0x64,0xac,0x08}, // cmovge 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x4E,0x64,0xac,0x08}, // cmovng 8(%r12,%r13,4), %r12
        {0x4f,0x0f,0x4F,0x64,0xac,0x08}  // cmovg 8(%r12,%r13,4), %r12
    };
    const auto generator = [](aasm::CondType type) {
        aasm::AsmEmitter a;
        a.cmov(8, type, aasm::Address(aasm::r12, aasm::r13,4, 8), aasm::r12);
        return a;
    };

    check_bytes_cc(codes, names, generator, cond_types);
}

TEST(Asm, lea) {
    const std::vector<std::string> names = {
        "leaq 8(%r12,%r13,4), %r12",
    };

    std::vector<std::vector<std::uint8_t>> codes = {
        {0x4f,0x8d,0x64,0xac,0x08}, // lea 8(%r12,%r13,4), %r12
    };

    const auto generator = [](const std::uint8_t) {
        aasm::AsmEmitter a;
        a.lea(aasm::r12, aasm::Address(aasm::r12, aasm::r13, 4, 8));
        return a;
    };

    std::array scale = {8};
    check_bytes(codes, names, generator, scale);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}