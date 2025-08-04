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
    ASSERT_EQ(fn(10, 20, 15), 15);
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
