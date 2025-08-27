#include <gtest/gtest.h>

#include <mir/mir.h>
#include "helpers/Jit.h"

static Module cvt(const IntegerType* from, const IntegerType* to) {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {from}, "cvt");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    const auto arg = data.arg(0);
    const auto cvt = data.sext(to, arg);
    data.ret(cvt);

    return builder.build();
}

const std::unordered_map<std::string, std::size_t> external_symbols;
const std::unordered_map<std::string, std::size_t> asm_size = {
    {"cvt", 2}
};

TEST(Convertion, sext_i8_to_i32) {
    const auto module = cvt(SignedIntegerType::i8(), SignedIntegerType::i32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<int(char)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<char>(-1)), -1);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(-128), -128);
}

TEST(Convertion, sext_i16_to_i32) {
    const auto module = cvt(SignedIntegerType::i16(), SignedIntegerType::i32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<int(short)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<short>(-1)), -1);
    ASSERT_EQ(fn(32767), 32767);
    ASSERT_EQ(fn(-32768), -32768);
}

TEST(Convertion, sext_i32_to_i64) {
    GTEST_SKIP();
    const auto module = cvt(SignedIntegerType::i32(), SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<long(int)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(-1), -1);
    ASSERT_EQ(fn(2147483647), 2147483647);
    ASSERT_EQ(fn(-2147483648), -2147483648);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}