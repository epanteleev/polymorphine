#include <gtest/gtest.h>
#include <climits>

#include <mir/mir.h>
#include "helpers/Jit.h"

static Module sext_cvt(const SignedIntegerType* from, const SignedIntegerType* to) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i32(), {from}, "cvt", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();

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
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i32(), {from}, "cvt", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();

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

static Module trunc_cvt(const IntegerType* from, const IntegerType* to) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(from, {from}, "cvt", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto data = fn_builder.value();

    const auto arg = data.arg(0);
    const auto cvt = data.trunc(to, arg);
    data.ret(cvt);

    return builder.build();
}

TEST(TruncConvertion, trunc_i32_to_i8) {
    const auto module = trunc_cvt(SignedIntegerType::i32(), SignedIntegerType::i8());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<char(int)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(-1), -1);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(-128), -128);
    ASSERT_EQ(fn(128), -128);
    ASSERT_EQ(fn(255), -1);
    ASSERT_EQ(fn(256), 0);
}

TEST(TruncConvertion, trunc_i32_to_i16) {
    const auto module = trunc_cvt(SignedIntegerType::i32(), SignedIntegerType::i16());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<short(int)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(-1), -1);
    ASSERT_EQ(fn(32767), 32767);
    ASSERT_EQ(fn(-32768), -32768);
    ASSERT_EQ(fn(32768), -32768);
    ASSERT_EQ(fn(65535), -1);
    ASSERT_EQ(fn(65536), 0);
}

TEST(TruncConvertion, trunc_i64_to_i8) {
    const auto module = trunc_cvt(SignedIntegerType::i64(), SignedIntegerType::i8());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<char(long)>("cvt").value();
    ASSERT_EQ(fn(1), 1);
    ASSERT_EQ(fn(-1), -1);
    ASSERT_EQ(fn(127), 127);
    ASSERT_EQ(fn(-128), -128);
    ASSERT_EQ(fn(128), -128);
    ASSERT_EQ(fn(255), -1);
    ASSERT_EQ(fn(256), 0);
    ASSERT_EQ(fn(9223372036854775807), -1);
    ASSERT_EQ(fn(-9223372036854775807L - 1), 0);
}

static Module bitcast_cvt(const IntegerType* from, const IntegerType* to) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(from, {from}, "cvt", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();

    const auto arg = data.arg(0);
    const auto cvt = data.bitcast(to, arg);
    data.ret(cvt);

    return builder.build();
}

TEST(BitcastConvertion, bitcast_i32_to_u32) {
    const auto module = bitcast_cvt(SignedIntegerType::i32(), UnsignedIntegerType::u32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned int(int)>("cvt").value();
    ASSERT_EQ(fn(1), 1U);
    ASSERT_EQ(fn(-1), 4294967295U);
    ASSERT_EQ(fn(127), 127U);
    ASSERT_EQ(fn(-128), 4294967168U);
}

TEST(BitcastConvertion, bitcast_u32_to_i32) {
    const auto module = bitcast_cvt(UnsignedIntegerType::u32(), SignedIntegerType::i32());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<int(unsigned int)>("cvt").value();
    ASSERT_EQ(fn(1U), 1);
    ASSERT_EQ(fn(4294967295U), -1);
    ASSERT_EQ(fn(127U), 127);
    ASSERT_EQ(fn(4294967168U), -128);
}

TEST(BitcastConvertion, bitcast_i64_to_u64) {
    const auto module = bitcast_cvt(SignedIntegerType::i64(), UnsignedIntegerType::u64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<unsigned long(long)>("cvt").value();
    ASSERT_EQ(fn(1L), 1UL);
    ASSERT_EQ(fn(-1L), 18446744073709551615UL);
    ASSERT_EQ(fn(127L), 127UL);
    ASSERT_EQ(fn(-128L), 18446744073709551488UL);
}

TEST(BitcastConvertion, bitcast_u64_to_i64) {
    const auto module = bitcast_cvt(UnsignedIntegerType::u64(), SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<long(unsigned long)>("cvt").value();
    ASSERT_EQ(fn(1UL), 1L);
    ASSERT_EQ(fn(18446744073709551615UL), -1L);
    ASSERT_EQ(fn(127UL), 127L);
    ASSERT_EQ(fn(18446744073709551488UL), -128L);
}

static Module fp2int_cvt(const FloatingPointType* from, const IntegerType* to) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(to, {from}, "cvt", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();

    const auto arg = data.arg(0);
    const auto cvt = data.fp2int(to, arg);
    data.ret(cvt);

    return builder.build();
}

TEST(Float2Int, fp2int_i64_to_f64) {
    GTEST_SKIP();
    const auto module = fp2int_cvt(FloatingPointType::f64(), SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, asm_size, true);
    const auto fn = code.code_start_as<long(double)>("cvt").value();
    ASSERT_EQ(fn(1.0), 1L);
    ASSERT_EQ(fn(18446744073709551615UL), -1L);
    ASSERT_EQ(fn(127UL), 127L);
    ASSERT_EQ(fn(18446744073709551488UL), -128L);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}