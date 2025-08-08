#include <gtest/gtest.h>

#include "utility/Error.h"

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module call_external() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "call_external");
    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto cont = data.create_basic_block();
    FunctionPrototype proto(SignedIntegerType::i32(), {}, "return_constant", FunctionLinkage::EXTERN);
    const auto res = data.call(std::move(proto), cont, {});
    data.switch_block(cont);
    data.ret(res);

    return builder.build();
}

int return_constant() {
    return 34;
}

TEST(CallTest, call_external) {
    std::unordered_map<std::string, std::size_t> external_symbols = {
        {"return_constant", reinterpret_cast<std::size_t>(&return_constant)}
    };

    const auto module = call_external();
    const auto code = jit_compile_and_assembly(external_symbols, module, true);
    const auto fn = code.code_start_as<std::int32_t()>("call_external").value();
    ASSERT_EQ(fn(), 34);
}

int add(const int a, const int b) {
    return a + b;
}

static Module call_external_with_args() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "call_external_with_args");
    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto cont = data.create_basic_block();
    FunctionPrototype proto(SignedIntegerType::i32(), {SignedIntegerType::i32(), SignedIntegerType::i32()}, "add", FunctionLinkage::EXTERN);
    const auto res = data.call(std::move(proto), cont, {Value::i32(10), Value::i32(20)});
    data.switch_block(cont);
    data.ret(res);
    return builder.build();
}

TEST(CallTest, call_external_with_args) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"add", reinterpret_cast<std::size_t>(&add)}
    };

    const auto module = call_external_with_args();
    const auto code = jit_compile_and_assembly(external_symbols, module, true);
    const auto fn = code.code_start_as<std::int32_t()>("call_external_with_args").value();
    ASSERT_EQ(fn(), 30);
}

long min(const long a, const long b) {
    return a < b ? a : b;
}

long max(const long a, const long b) {
    return a > b ? a : b;
}

static Module clamp() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()}, "clamp");
    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto a = data.arg(0);
    const auto min_val = data.arg(1);
    const auto max_val = data.arg(2);

    const auto cont = data.create_basic_block();
    FunctionPrototype proto_min(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "min", FunctionLinkage::EXTERN);
    const auto clamped = data.call(std::move(proto_min), cont, {a, max_val});
    data.switch_block(cont);

    auto cont1 = data.create_basic_block();
    FunctionPrototype proto_max(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "max", FunctionLinkage::EXTERN);
    const auto clamped_final = data.call(std::move(proto_max), cont1, {clamped, min_val});
    data.switch_block(cont1);

    data.ret(clamped_final);
    return builder.build();
}

TEST(CallTest, clamp) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"min", reinterpret_cast<std::size_t>(&min)},
        {"max", reinterpret_cast<std::size_t>(&max)}
    };

    const auto module = clamp();
    const auto code = jit_compile_and_assembly(external_symbols, module, true);
    const auto fn = code.code_start_as<long(long, long, long)>("clamp").value();
    ASSERT_EQ(fn(10, 5, 15), 10);
    ASSERT_EQ(fn(20, 5, 15), 15);
    ASSERT_EQ(fn(10, 15, 20), 15);
}

static Module clamp2() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()}, "clamp");
    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto a = data.arg(0);
    const auto min_val = data.arg(1);
    const auto max_val = data.arg(2);

    auto cont1 = data.create_basic_block();
    FunctionPrototype proto_max(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "max", FunctionLinkage::EXTERN);
    const auto clamped = data.call(std::move(proto_max), cont1, {a, min_val});
    data.switch_block(cont1);

    const auto cont = data.create_basic_block();
    FunctionPrototype proto_min(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "min", FunctionLinkage::EXTERN);
    const auto clamped_final = data.call(std::move(proto_min), cont, {clamped, max_val});
    data.switch_block(cont);

    data.ret(clamped_final);
    return builder.build();
}

TEST(CallTest, clamp2) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"min", reinterpret_cast<std::size_t>(&min)},
        {"max", reinterpret_cast<std::size_t>(&max)}
    };

    const auto module = clamp2();
    const auto code = jit_compile_and_assembly(external_symbols, module, true);
    const auto fn = code.code_start_as<long(long, long, long)>("clamp").value();
    ASSERT_EQ(fn(10, 5, 15), 10);
    ASSERT_EQ(fn(20, 5, 15), 15);
    ASSERT_EQ(fn(10, 15, 20), 15);
}

static long arg_locator(int a0, int a1, int a2, int a3, int a4, int a5) {
    long ret = 0;
    ret |= static_cast<long>(a0) << 0;
    ret |= static_cast<long>(a1) << 8;
    ret |= static_cast<long>(a2) << 16;
    ret |= static_cast<long>(a3) << 24;
    ret |= static_cast<long>(a4) << 32;
    ret |= static_cast<long>(a5) << 40;
    return ret;
}

static long arg_shuffle(int a0, int a1, int a2, int a3, int a4, int a5) {
    return arg_locator(a5, a4, a3, a2, a1, a0);
}

static Module argument_shuffle() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i64(), {SignedIntegerType::i32(), SignedIntegerType::i32(), SignedIntegerType::i32(),
                                                          SignedIntegerType::i32(), SignedIntegerType::i32(), SignedIntegerType::i32()}, "arg_shuffle");
    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto a0 = data.arg(5);
    const auto a1 = data.arg(4);
    const auto a2 = data.arg(3);
    const auto a3 = data.arg(2);
    const auto a4 = data.arg(1);
    const auto a5 = data.arg(0);

    FunctionPrototype arg_locator(SignedIntegerType::i64(), {SignedIntegerType::i32(), SignedIntegerType::i32(),
                                                                               SignedIntegerType::i32(), SignedIntegerType::i32(),
                                                                               SignedIntegerType::i32(), SignedIntegerType::i32()},
                                                     "arg_locator", FunctionLinkage::EXTERN);

    const auto cont = data.create_basic_block();
    const auto ret_val = data.call(std::move(arg_locator), cont, {a0, a1, a2, a3, a4, a5});
    data.switch_block(cont);

    data.ret(ret_val);
    return builder.build();
}

TEST(CallTest, argument_shuffle) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"arg_locator", reinterpret_cast<std::size_t>(&arg_locator)}
    };

    const auto module = argument_shuffle();
    const auto code = jit_compile_and_assembly(external_symbols, module, true);
    const auto fn = code.code_start_as<long(int, int, int, int, int, int)>("arg_shuffle").value();
    ASSERT_EQ(fn(1, 2, 3, 4, 5, 6), arg_shuffle(1, 2, 3, 4, 5, 6));
}

static Module argument_shuffle1() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64(),
                                                          SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()}, "arg_shuffle");
    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto aa = data.arg(6);
    const auto a0 = data.arg(5);
    const auto a1 = data.arg(4);
    const auto a2 = data.arg(3);
    const auto a3 = data.arg(2);
    const auto a4 = data.arg(1);
    const auto a5 = data.arg(0);

    FunctionPrototype arg_locator(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64(),
                                                          SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()},
                                                     "arg_locator1", FunctionLinkage::EXTERN);

    const auto cont = data.create_basic_block();
    const auto ret_val = data.call(std::move(arg_locator), cont, {aa, a0, a1, a2, a3, a4, a5});
    data.switch_block(cont);

    data.ret(ret_val);
    return builder.build();
}

static long arg_locator1(long a0, long a1, long a2, long a3, long a4, long a5, long a6) {
    long ret = 0;
    ret |= a0 << 0;
    ret |= a1 << 8;
    ret |= a2 << 16;
    ret |= a3 << 24;
    ret |= a4 << 32;
    ret |= a5 << 40;
    ret |= a6 << 48;
    return ret;
}

static long arg_shuffle1(long a0, long a1, long a2, long a3, long a4, long a5, long a6) {
    return arg_locator1(a6, a5, a4, a3, a2, a1, a0);
}

TEST(CallTest, argument_shuffle1) {
    GTEST_SKIP();
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"arg_locator1", reinterpret_cast<std::size_t>(&arg_locator1)}
    };

    const auto module = argument_shuffle1();
    const auto code = jit_compile_and_assembly(external_symbols, module, true);
    const auto fn = code.code_start_as<long(long, long, long, long, long, long, long)>("arg_shuffle").value();
    ASSERT_EQ(fn(1, 2, 3, 4, 5, 6, 7), arg_shuffle1(1, 2, 3, 4, 5, 6, 7));
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}