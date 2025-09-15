#include <gtest/gtest.h>

#include "utility/Error.h"

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module call_external() {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i32(), {}, "call_external", FunctionBind::DEFAULT);
    auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto cont = data.create_basic_block();
    const auto proto = builder.add_function_prototype(SignedIntegerType::i32(), {}, "return_constant", FunctionBind::EXTERN);
    const auto res = data.call(proto, cont, {});
    data.switch_block(cont);
    data.ret(res);

    return builder.build();
}

int return_constant() {
    return 34;
}

TEST(CallTest, call_external) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
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
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i32(), {}, "call_external_with_args", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto cont = data.create_basic_block();
    const auto proto = builder.add_function_prototype(SignedIntegerType::i32(), {SignedIntegerType::i32(), SignedIntegerType::i32()}, "add", FunctionBind::EXTERN);
    const auto res = data.call(proto, cont, {Value::i32(10), Value::i32(20)});
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
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()}, "clamp", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto a = data.arg(0);
    const auto min_val = data.arg(1);
    const auto max_val = data.arg(2);

    const auto cont = data.create_basic_block();
    const auto proto_min = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "min", FunctionBind::EXTERN);
    const auto clamped = data.call(proto_min, cont, {a, max_val});
    data.switch_block(cont);

    const auto cont1 = data.create_basic_block();
    const auto proto_max = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "max", FunctionBind::EXTERN);
    const auto clamped_final = data.call(proto_max, cont1, {clamped, min_val});
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
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()}, "clamp", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto a = data.arg(0);
    const auto min_val = data.arg(1);
    const auto max_val = data.arg(2);

    auto cont1 = data.create_basic_block();
    const auto proto_max = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "max", FunctionBind::EXTERN);
    const auto clamped = data.call(proto_max, cont1, {a, min_val});
    data.switch_block(cont1);

    const auto cont = data.create_basic_block();
    const auto proto_min = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), SignedIntegerType::i64()}, "min", FunctionBind::EXTERN);
    const auto clamped_final = data.call(proto_min, cont, {clamped, max_val});
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
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i32(), SignedIntegerType::i32(), SignedIntegerType::i32(),
                                                          SignedIntegerType::i32(), SignedIntegerType::i32(), SignedIntegerType::i32()}, "arg_shuffle", FunctionBind::DEFAULT);
    auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto a0 = data.arg(5);
    const auto a1 = data.arg(4);
    const auto a2 = data.arg(3);
    const auto a3 = data.arg(2);
    const auto a4 = data.arg(1);
    const auto a5 = data.arg(0);

    const auto arg_locator = builder.add_function_prototype(SignedIntegerType::i64(), {SignedIntegerType::i32(), SignedIntegerType::i32(),
                                                                               SignedIntegerType::i32(), SignedIntegerType::i32(),
                                                                               SignedIntegerType::i32(), SignedIntegerType::i32()},
                                                     "arg_locator", FunctionBind::EXTERN);

    const auto cont = data.create_basic_block();
    const auto ret_val = data.call(arg_locator, cont, {a0, a1, a2, a3, a4, a5});
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

static Module argument_shuffle7(const IntegerType* ty) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {ty, ty, ty, ty, ty, ty, ty}, "arg_shuffle", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto aa = data.arg(6);
    const auto a0 = data.arg(5);
    const auto a1 = data.arg(4);
    const auto a2 = data.arg(3);
    const auto a3 = data.arg(2);
    const auto a4 = data.arg(1);
    const auto a5 = data.arg(0);

    const auto arg_locator = builder.add_function_prototype(SignedIntegerType::i64(), {ty, ty, ty, ty, ty, ty, ty}, "arg_locator7", FunctionBind::EXTERN);
    const auto cont = data.create_basic_block();
    const auto ret_val = data.call(arg_locator, cont, {aa, a0, a1, a2, a3, a4, a5});
    data.switch_block(cont);

    data.ret(ret_val);
    return builder.build();
}

template<std::integral T>
static long arg_locator7(T a0, T a1, T a2, T a3, T a4, T a5, T a6) {
    long ret = 0;
    ret |= static_cast<long>(a0) << 0;
    ret |= static_cast<long>(a1) << 8;
    ret |= static_cast<long>(a2) << 16;
    ret |= static_cast<long>(a3) << 24;
    ret |= static_cast<long>(a4) << 32;
    ret |= static_cast<long>(a5) << 40;
    ret |= static_cast<long>(a6) << 48;
    return ret;
}

template<std::integral T>
static long arg_shuffle7(T a0, T a1, T a2, T a3, T a4, T a5, T a6) {
    return arg_locator7<T>(a6, a5, a4, a3, a2, a1, a0);
}

static const std::unordered_map<std::string, std::size_t> arg_shuffle7_sizes = {
    {"arg_shuffle", 16}
};

TEST(CallTest, argument_shuffle7_i64) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"arg_locator7", reinterpret_cast<std::size_t>(&arg_locator7<long>)}
    };

    const auto module = argument_shuffle7(SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, arg_shuffle7_sizes, true);
    const auto fn = code.code_start_as<long(long, long, long, long, long, long, long)>("arg_shuffle").value();
    std::cout << arg_shuffle7<long>(1L, 2L, 3L, 4L, 5L, 6L, 7L);
    ASSERT_EQ(fn(1, 2, 3, 4, 5, 6, 7), arg_shuffle7<long>(1L, 2L, 3L, 4L, 5L, 6L, 7L));
}

TEST(CallTest, argument_shuffle7_i32) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"arg_locator7", reinterpret_cast<std::size_t>(&arg_locator7<int>)}
    };

    const auto module = argument_shuffle7(SignedIntegerType::i32());
    const auto code = jit_compile_and_assembly(external_symbols, module, arg_shuffle7_sizes, true);
    const auto fn = code.code_start_as<long(int, int, int, int, int, int, int)>("arg_shuffle").value();
    std::cout << arg_shuffle7<int>(1, 2, 3, 4, 5, 6, 7);
    ASSERT_EQ(fn(1, 2, 3, 4, 5, 6, 7), arg_shuffle7<int>(1, 2, 3, 4, 5, 6, 7));
}

TEST(CallTest, argument_shuffle7_i16) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"arg_locator7", reinterpret_cast<std::size_t>(&arg_locator7<short>)}
    };

    const auto module = argument_shuffle7(SignedIntegerType::i16());
    const auto code = jit_compile_and_assembly(external_symbols, module, arg_shuffle7_sizes, true);
    const auto fn = code.code_start_as<long(short, short, short, short, short, short, short)>("arg_shuffle").value();
    ASSERT_EQ(fn(1, 2, 3, 4, 5, 6, 7), arg_shuffle7<short>(1, 2, 3, 4, 5, 6, 7));
}

TEST(CallTest, argument_shuffle7_i8) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"arg_locator7", reinterpret_cast<std::size_t>(&arg_locator7<char>)}
    };

    const auto module = argument_shuffle7(SignedIntegerType::i8());
    const auto code = jit_compile_and_assembly(external_symbols, module, arg_shuffle7_sizes, true);
    const auto fn = code.code_start_as<long(char, char, char, char, char, char, char)>("arg_shuffle").value();
    ASSERT_EQ(fn(1, 2, 3, 4, 5, 6, 7), arg_shuffle7<char>(1, 2, 3, 4, 5, 6, 7));
}

static Module argument_shuffle8(const IntegerType* ty) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {ty, ty, ty, ty, ty, ty, ty, ty}, "arg_shuffle", FunctionBind::DEFAULT);
    auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto ab = data.arg(7);
    const auto aa = data.arg(6);
    const auto a0 = data.arg(5);
    const auto a1 = data.arg(4);
    const auto a2 = data.arg(3);
    const auto a3 = data.arg(2);
    const auto a4 = data.arg(1);
    const auto a5 = data.arg(0);

    const auto arg_locator = builder.add_function_prototype(SignedIntegerType::i64(), {ty, ty, ty, ty, ty, ty, ty, ty, ty}, "arg_locator8", FunctionBind::EXTERN);
    const auto cont = data.create_basic_block();
    const auto ret_val = data.call(arg_locator, cont, {ab, aa, a0, a1, a2, a3, a4, a5});
    data.switch_block(cont);

    data.ret(ret_val);
    return builder.build();
}

template<std::integral T>
static long arg_locator8(T a0, T a1, T a2, T a3, T a4, T a5, T a6, T a7) {
    long ret = 0;
    ret |= a0 << 0;
    ret |= a1 << 8;
    ret |= a2 << 16;
    ret |= a3 << 24;
    ret |= a4 << 32;
    ret |= a5 << 40;
    ret |= a6 << 48;
    ret |= a7 << 56;
    return ret;
}

template<std::integral T>
static long arg_shuffle8(T a0, T a1, T a2, T a3, T a4, T a5, T a6, T a7) {
    return arg_locator8(a7, a6, a5, a4, a3, a2, a1, a0);
}

static const std::unordered_map<std::string, std::size_t> arg_shuffl8_sizes = {
    {"arg_shuffle", 21}
};

TEST(CallTest, argument_shuffle8_i64) {
    const std::unordered_map<std::string, std::size_t> external_symbols = {
        {"arg_locator8", reinterpret_cast<std::size_t>(&arg_locator8<long>)}
    };

    const auto module = argument_shuffle8(SignedIntegerType::i64());
    const auto code = jit_compile_and_assembly(external_symbols, module, arg_shuffl8_sizes, true);
    const auto fn = code.code_start_as<long(long, long, long, long, long, long, long, long)>("arg_shuffle").value();
    ASSERT_EQ(fn(1, 2, 3, 4, 5, 6, 7, 8), arg_shuffle8(1L, 2L, 3L, 4L, 5L, 6L, 7L, 8L));
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}