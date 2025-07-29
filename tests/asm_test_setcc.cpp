#include <gtest/gtest.h>

#include "helpers/Utils.h"
#include "asm/asm.h"

template<std::ranges::range R>
[[maybe_unused]]
static void check_bytes_setcc(const std::vector<std::vector<std::uint8_t>>& codes, const std::vector<std::string>& names, aasm::AsmEmitter(*fn)(aasm::CondType type), R&& scales) {
    ASSERT_EQ(codes.size(), names.size());
    ASSERT_GT(codes.size(), 0U) << "No codes provided for testing";

    for (const auto& [idx, scale] : std::ranges::views::enumerate(scales)) {
        aasm::AsmEmitter a = fn(scale);
        const auto asm_buffer = a.to_buffer();
        std::uint8_t v[aasm::constants::MAX_X86_INSTRUCTION_SIZE]{};
        const auto size = to_byte_buffer(asm_buffer, v);
        auto& code = codes[idx];

        ASSERT_EQ(size, code.size()) << "Mismatch at scale=" << scale;
        for (std::size_t i = 0; i < code.size(); ++i) {
            ASSERT_EQ(v[i], code[i]) << "Mismatch at index=" << i << " scale=" << scale;
        }

        auto& name = names[idx];
        ASSERT_EQ(name, make_string(asm_buffer)) << "Mismatch at scale=" << scale;
    }
}

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

TEST(Asm, setcc1) {
    std::vector<std::string> names = {
        "seto %r12b",
        "setno %r12b",
        "setnae %r12b",
        "setae %r12b",
        "sete %r12b",
        "setne %r12b",
        "setna %r12b",
        "seta %r12b",
        "sets %r12b",
        "setns %r12b",
        "setp %r12b",
        "setnp %r12b",
        "setnge %r12b",
        "setge %r12b",
        "setng %r12b",
        "setg %r12b"
    };
    std::vector<std::vector<std::uint8_t>> codes = {
        {0x41,0x0f,0x90,0xc4}, // seto %r12b
        {0x41,0x0f,0x91,0xc4}, // setno %r12b
        {0x41,0x0f,0x92,0xc4}, // setnae %r12b
        {0x41,0x0f,0x93,0xc4}, // setae %r12b
        {0x41,0x0f,0x94,0xc4}, // sete %r12b
        {0x41,0x0f,0x95,0xc4}, // setne %r12b
        {0x41,0x0f,0x96,0xc4}, // setna %r12b
        {0x41,0x0f,0x97,0xc4}, // seta %r12b
        {0x41,0x0f,0x98,0xc4}, // sets %r12b
        {0x41,0x0f,0x99,0xc4}, // setns %r12b
        {0x41,0x0f,0x9a,0xc4}, // setp %r12b
        {0x41,0x0f,0x9b,0xc4}, // setnp %r12b
        {0x41,0x0f,0x9c,0xc4}, // setnge %r12b
        {0x41,0x0f,0x9d,0xc4}, // setge %r12b
        {0x41,0x0f,0x9e,0xc4}, // setng %r12b
        {0x41,0x0f,0x9f,0xc4}  // setg %r12b
    };

    const auto generator = [](aasm::CondType type) {
        aasm::AsmEmitter a;
        a.setcc(type, aasm::r12);
        return a;
    };

    check_bytes_setcc(codes, names, generator, cond_types);
}

TEST(Asm, setcc2) {
    std::vector<std::string> names = {
        "seto %cl",
        "setno %cl",
        "setnae %cl",
        "setae %cl",
        "sete %cl",
        "setne %cl",
        "setna %cl",
        "seta %cl",
        "sets %cl",
        "setns %cl",
        "setp %cl",
        "setnp %cl",
        "setnge %cl",
        "setge %cl",
        "setng %cl",
        "setg %cl"
    };

    std::vector<std::vector<std::uint8_t>> codes = {
        {0x0f,0x90,0xc1}, // seto %cl
        {0x0f,0x91,0xc1}, // setno %cl
        {0x0f,0x92,0xc1}, // setnae %cl
        {0x0f,0x93,0xc1}, // setae %cl
        {0x0f,0x94,0xc1}, // sete %cl
        {0x0f,0x95,0xc1}, // setne %cl
        {0x0f,0x96,0xc1}, // setna %cl
        {0x0f,0x97,0xc1}, // seta %cl
        {0x0f,0x98,0xc1}, // sets %cl
        {0x0f,0x99,0xc1}, // setns %cl
        {0x0f,0x9a,0xc1}, // setp %cl
        {0x0f,0x9b,0xc1}, // setnp %cl
        {0x0f,0x9c,0xc1}, // setnge %cl
        {0x0f,0x9d,0xc1}, // setge %cl
        {0x0f,0x9e,0xc1}, // setng %cl
        {0x0f,0x9f,0xc1}  // setg %cl
    };

    const auto generator = [](aasm::CondType type) {
        aasm::AsmEmitter a;
        a.setcc(type, aasm::rcx);
        return a;
    };

    check_bytes_setcc(codes, names, generator, cond_types);
}

TEST(Asm, setcc3) {
    std::vector<std::string> names = {
        "seto %dil",
        "setno %dil",
        "setnae %dil",
        "setae %dil",
        "sete %dil",
        "setne %dil",
        "setna %dil",
        "seta %dil",
        "sets %dil",
        "setns %dil",
        "setp %dil",
        "setnp %dil",
        "setnge %dil",
        "setge %dil",
        "setng %dil",
        "setg %dil"
    };

    std::vector<std::vector<std::uint8_t>> codes = {
      {0x40,0x0f,0x90,0xc7}, // seto %dil
      {0x40,0x0f,0x91,0xc7}, // setno %dil
      {0x40,0x0f,0x92,0xc7}, // setnae %dil
      {0x40,0x0f,0x93,0xc7}, // setae %dil
      {0x40,0x0f,0x94,0xc7}, // sete %dil
      {0x40,0x0f,0x95,0xc7}, // setne %dil
      {0x40,0x0f,0x96,0xc7}, // setna %dil
      {0x40,0x0f,0x97,0xc7}, // seta %dil
      {0x40,0x0f,0x98,0xc7}, // sets %dil
      {0x40,0x0f,0x99,0xc7}, // setns %dil
      {0x40,0x0f,0x9a,0xc7}, // setp %dil
      {0x40,0x0f,0x9b,0xc7}, // setnp %dil
      {0x40,0x0f,0x9c,0xc7}, // setnge %dil
      {0x40,0x0f,0x9d,0xc7}, // setge %dil
      {0x40,0x0f,0x9e,0xc7}, // setng %dil
      {0x40,0x0f,0x9f,0xc7}   // setg %dil
    };

    const auto generator = [](aasm::CondType type) {
        aasm::AsmEmitter a;
        a.setcc(type, aasm::rdi);
        return a;
    };

    check_bytes_setcc(codes, names, generator, cond_types);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}