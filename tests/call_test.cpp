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


static Module clamp(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "max", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        auto data = fn_builder.value();
        const auto arg1 = data.arg(0);
        const auto arg2 = data.arg(1);
        const auto alloc = data.alloc(ty);
        const auto max = data.icmp(IcmpPredicate::Gt, arg1, arg2);
        const auto then = data.create_basic_block();
        const auto else_ = data.create_basic_block();
        const auto cont = data.create_basic_block();
        data.br_cond(max, then, else_);

        data.switch_block(then);
        data.store(alloc, arg1);
        data.br(cont);

        data.switch_block(else_);
        data.store(alloc, arg2);
        data.br(cont);
        data.switch_block(cont);
        data.ret(data.load(ty, alloc));
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "min", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        auto data = fn_builder.value();
        const auto arg1 = data.arg(0);
        const auto arg2 = data.arg(1);
        const auto alloc = data.alloc(ty);
        const auto min = data.icmp(IcmpPredicate::Lt, arg1, arg2);
        const auto then = data.create_basic_block();
        const auto else_ = data.create_basic_block();
        const auto cont = data.create_basic_block();
        data.br_cond(min, then, else_);

        data.switch_block(then);
        data.store(alloc, arg1);
        data.br(cont);

        data.switch_block(else_);
        data.store(alloc, arg2);
        data.br(cont);
        data.switch_block(cont);
        data.ret(data.load(ty, alloc));
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty, ty}, "clamp", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        auto data = fn_builder.value();
        const auto arg = data.arg(0);
        const auto min = data.arg(1);
        const auto max = data.arg(2);
        const auto cont = data.create_basic_block();
        const auto max_proto = builder.add_function_prototype(ty, {ty, ty}, "max", FunctionBind::DEFAULT);
        const auto min_val = data.call(max_proto, cont, {arg, min});
        data.switch_block(cont);
        const auto then = data.create_basic_block();
        const auto min_proto = builder.add_function_prototype(ty, {ty, ty}, "min", FunctionBind::DEFAULT);
        const auto max_val = data.call(min_proto, then, {min_val, max});
        data.switch_block(then);
        data.ret(max_val);
    }
    return builder.build();
}

template<typename Fn>
static void verify_clamp(const Module& module) {
    const std::unordered_map<std::string, std::size_t> sizes = {
        {"max", 11},
        {"min", 11},
        {"clamp", 9}
    };

    const auto code = jit_compile_and_assembly(module, sizes, true);
    const auto clamp = code.code_start_as<Fn>("clamp").value();

    ASSERT_EQ(clamp(10, 0, 20), 10);
    ASSERT_EQ(clamp(10, 20, 30), 20);
    ASSERT_EQ(clamp(10, 5, 15), 10);
    ASSERT_EQ(clamp(10, 10, 10), 10);
    ASSERT_EQ(clamp(10, -5, 5), 5);
}

TEST(CallTest, clamp_i32) {
    const auto module = clamp(SignedIntegerType::i32());
    verify_clamp<std::int32_t(std::int32_t, std::int32_t, std::int32_t)>(module);
}

TEST(CallTest, clamp_i64) {
    const auto module = clamp(SignedIntegerType::i64());
    verify_clamp<std::int64_t(std::int64_t, std::int64_t, std::int64_t)>(module);
}

TEST(CallTest, clamp_u32) {
    const auto module = clamp(UnsignedIntegerType::u32());
    verify_clamp<std::uint32_t(std::uint32_t, std::uint32_t, std::uint32_t)>(module);
}

TEST(CallTest, clamp_u64) {
    const auto module = clamp(UnsignedIntegerType::u64());
    verify_clamp<std::uint64_t(std::uint64_t, std::uint64_t, std::uint64_t)>(module);
}

TEST(CallTest, clamp_u8) {
    const auto module = clamp(UnsignedIntegerType::u8());
    verify_clamp<std::uint8_t(std::uint8_t, std::uint8_t, std::uint8_t)>(module);
}

TEST(CallTest, clamp_i8) {
    const auto module = clamp(SignedIntegerType::i8());
    verify_clamp<std::int8_t(std::int8_t, std::int8_t, std::int8_t)>(module);
}

TEST(CallTest, clamp_i16) {
    const auto module = clamp(SignedIntegerType::i16());
    verify_clamp<std::int16_t(std::int16_t, std::int16_t, std::int16_t)>(module);
}

TEST(CallTest, clamp_u16) {
    const auto module = clamp(UnsignedIntegerType::u16());
    verify_clamp<std::uint16_t(std::uint16_t, std::uint16_t, std::uint16_t)>(module);
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}