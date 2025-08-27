#include <gtest/gtest.h>

#include <mir/mir.h>
#include "helpers/Jit.h"

static Module sext_cvt(const SignedIntegerType* from, const SignedIntegerType* to) {
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

TEST(SignConvertion, sext_i8_to_i32) {
    const auto module = sext_cvt(SignedIntegerType::i8(), SignedIntegerType::i32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<int(char)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<char>(-1)), -1);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(-128), -128);
}

TEST(SignConvertion, sext_i16_to_i32) {
    const auto module = sext_cvt(SignedIntegerType::i16(), SignedIntegerType::i32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<int(short)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<short>(-1)), -1);
    ASSERT_EQ(fn(32767), 32767);
    ASSERT_EQ(fn(-32768), -32768);
}

TEST(SignConvertion, sext_i32_to_i64) {
    const auto module = sext_cvt(SignedIntegerType::i32(), SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<long(int)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(-1), -1);
    ASSERT_EQ(fn(INT_MAX), INT_MAX);
    ASSERT_EQ(fn(-2147483648), -2147483648);
}

TEST(SignConvertion, sext_i16_to_i64) {
    const auto module = sext_cvt(SignedIntegerType::i16(), SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<long(short)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<short>(-1)), -1);
    ASSERT_EQ(fn(32767), 32767);
    ASSERT_EQ(fn(-32768), -32768);
}

TEST(SignConvertion, sext_i8_to_i64) {
    const auto module = sext_cvt(SignedIntegerType::i8(), SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<long(char)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<char>(-1)), -1);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(-128), -128);
}

TEST(SignConvertion, sext_i8_to_i16) {
    const auto module = sext_cvt(SignedIntegerType::i8(), SignedIntegerType::i16());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<short(char)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<char>(-1)), -1);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(-128), -128);
}

static Module zext_cvt(const UnsignedIntegerType* from, const UnsignedIntegerType* to) {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {from}, "cvt");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    const auto arg = data.arg(0);
    const auto cvt = data.zext(to, arg);
    data.ret(cvt);

    return builder.build();
}

TEST(ZeroConvertion, zext_u8_to_u32) {
    const auto module = zext_cvt(UnsignedIntegerType::u8(), UnsignedIntegerType::u32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned int(unsigned char)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<unsigned char>(255)), 255);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(0), 0);
}

TEST(ZeroConvertion, zext_u16_to_u32) {
    const auto module = zext_cvt(UnsignedIntegerType::u16(), UnsignedIntegerType::u32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned int(unsigned short)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<unsigned short>(65535)), 65535);
    ASSERT_EQ(fn(32767), 32767);
    ASSERT_EQ(fn(0), 0);
}

TEST(ZeroConvertion, zext_u32_to_u64) {
    const auto module = zext_cvt(UnsignedIntegerType::u32(), UnsignedIntegerType::u64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned long(unsigned int)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(4294967295), 4294967295);
    ASSERT_EQ(fn(2147483647), 2147483647);
    ASSERT_EQ(fn(0), 0);
}

TEST(ZeroConvertion, zext_u16_to_u64) {
    const auto module = zext_cvt(UnsignedIntegerType::u16(), UnsignedIntegerType::u64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned long(unsigned short)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<unsigned short>(65535)), 65535);
    ASSERT_EQ(fn(32767), 32767);
    ASSERT_EQ(fn(0), 0);
}

TEST(ZeroConvertion, zext_u8_to_u64) {
    const auto module = zext_cvt(UnsignedIntegerType::u8(), UnsignedIntegerType::u64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned long(unsigned char)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<unsigned char>(255)), 255);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(0), 0);
}

TEST(ZeroConvertion, zext_u8_to_u16) {
    const auto module = zext_cvt(UnsignedIntegerType::u8(), UnsignedIntegerType::u16());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned short(unsigned char)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(static_cast<unsigned char>(255)), 255);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(0), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}