#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static void call_test(ModuleBuilder& builder, const IntegerType* ty) {
    const auto prototype = builder.add_function_prototype(ty, {}, "call_test", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto cont = data.create_basic_block();
    const auto proto = builder.add_function_prototype(ty, {}, "ret_42", FunctionBind::DEFAULT);
    const auto res = data.call(proto, cont, {});
    data.switch_block(cont);
    data.ret(res);
}

template<typename Fn>
static void ret_42(ModuleBuilder& builder, const IntegerType* ty, Fn&& fn) {
    const auto ret_42 = builder.add_function_prototype(ty, {}, "ret_42", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(ret_42);
    const auto data = fn_builder.value();
    data.ret(fn(42));
}

template<typename Fn>
static Module create_wrappers(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    call_test(builder, ty);
    ret_42(builder, ty, fn);
    return builder.build();
}

template<typename Fn>
static Module create_wrappers_reversed(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    ret_42(builder, ty, fn);
    call_test(builder, ty);
    return builder.build();
}

TEST(CallTest, call_i32) {
    const auto module = create_wrappers(SignedIntegerType::i32(), Value::i32);
    auto code = jit_compile_and_assembly(module, true);
    const auto fn = code.code_start_as<std::int32_t()>("call_test").value();
    ASSERT_EQ(fn(), 42);
}

TEST(CallTest, call_i32_reversed) {
    const auto module = create_wrappers_reversed(SignedIntegerType::i32(), Value::i32);
    auto code = jit_compile_and_assembly(module);
    const auto fn = code.code_start_as<std::int32_t()>("call_test").value();
    ASSERT_EQ(fn(), 42);
}

TEST(CallTest, call_i64) {
    const auto module = create_wrappers(SignedIntegerType::i64(), Value::i64);
    auto code = jit_compile_and_assembly(module);
    const auto fn = code.code_start_as<std::int64_t()>("call_test").value();
    ASSERT_EQ(fn(), 42);
}

TEST(CallTest, call_u32) {
    const auto module = create_wrappers(UnsignedIntegerType::u32(), Value::u32);
    auto code = jit_compile_and_assembly(module);
    const auto fn = code.code_start_as<std::uint32_t()>("call_test").value();
    ASSERT_EQ(fn(), 42);
}

TEST(CallTest, call_u64) {
    const auto module = create_wrappers(UnsignedIntegerType::u64(), Value::u64);
    auto code = jit_compile_and_assembly(module);
    const auto fn = code.code_start_as<std::uint64_t()>("call_test").value();
    ASSERT_EQ(fn(), 42);
}

static Module return_arg(const IntegerType* ty, const Value& val) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "return_arg", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        auto data = fn_builder.value();
        const auto cont = data.create_basic_block();
        const auto arg = data.arg(0);
        const auto proto = builder.add_function_prototype(ty, {ty, ty}, "sum", FunctionBind::DEFAULT);
        const auto call = data.call(proto, cont, {arg, val});
        data.switch_block(cont);
        data.ret(call);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "sum", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        const auto data = fn_builder.value();
        const auto arg1 = data.arg(0);
        const auto arg2 = data.arg(1);
        const auto sum = data.add(arg1, arg2);
        data.ret(sum);
    }

    return builder.build();
}

TEST(CallTest, return_arg_i32) {
    const auto module = return_arg(SignedIntegerType::i32(), Value::i32(10));
    const auto code = jit_compile_and_assembly(module);
    const auto fn = code.code_start_as<std::int32_t(std::int32_t)>("return_arg").value();
    ASSERT_EQ(fn(32), 42);
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}