#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module clamp_int(const IntegerType* ty) {
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
static void verify_clamp_int(const Module& module) {
    const std::unordered_map<std::string, std::size_t> sizes = {
        {"max", 11},
        {"min", 11},
        {"clamp", 5}
    };

    const auto code = jit_compile_and_assembly(module, sizes, true);
    const auto clamp = code.code_start_as<Fn>("clamp").value();

    ASSERT_EQ(clamp(10, 0, 20), 10);
    ASSERT_EQ(clamp(10, 20, 30), 20);
    ASSERT_EQ(clamp(10, 5, 15), 10);
    ASSERT_EQ(clamp(10, 10, 10), 10);
    ASSERT_EQ(clamp(10, -5, 5), 5);
}

TEST(Clamp, clamp_i32) {
    const auto module = clamp_int(SignedIntegerType::i32());
    verify_clamp_int<std::int32_t(std::int32_t, std::int32_t, std::int32_t)>(module);
}

TEST(Clamp, clamp_i64) {
    const auto module = clamp_int(SignedIntegerType::i64());
    verify_clamp_int<std::int64_t(std::int64_t, std::int64_t, std::int64_t)>(module);
}

TEST(Clamp, clamp_u32) {
    const auto module = clamp_int(UnsignedIntegerType::u32());
    verify_clamp_int<std::uint32_t(std::uint32_t, std::uint32_t, std::uint32_t)>(module);
}

TEST(Clamp, clamp_u64) {
    const auto module = clamp_int(UnsignedIntegerType::u64());
    verify_clamp_int<std::uint64_t(std::uint64_t, std::uint64_t, std::uint64_t)>(module);
}

TEST(Clamp, clamp_u8) {
    const auto module = clamp_int(UnsignedIntegerType::u8());
    verify_clamp_int<std::uint8_t(std::uint8_t, std::uint8_t, std::uint8_t)>(module);
}

TEST(Clamp, clamp_i8) {
    const auto module = clamp_int(SignedIntegerType::i8());
    verify_clamp_int<std::int8_t(std::int8_t, std::int8_t, std::int8_t)>(module);
}

TEST(Clamp, clamp_i16) {
    const auto module = clamp_int(SignedIntegerType::i16());
    verify_clamp_int<std::int16_t(std::int16_t, std::int16_t, std::int16_t)>(module);
}

TEST(Clamp, clamp_u16) {
    const auto module = clamp_int(UnsignedIntegerType::u16());
    verify_clamp_int<std::uint16_t(std::uint16_t, std::uint16_t, std::uint16_t)>(module);
}

static Module clamp_float(const FloatingPointType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "max", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        auto data = fn_builder.value();
        const auto arg1 = data.arg(0);
        const auto arg2 = data.arg(1);
        const auto alloc = data.alloc(ty);
        const auto max = data.fcmp(FcmpPredicate::Oge, arg1, arg2);
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
        const auto min = data.fcmp(FcmpPredicate::Olt, arg1, arg2);
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
static void verify_clamp_float(const Module& module) {
    const std::unordered_map<std::string, std::size_t> sizes = {
        {"max", 11},
        {"min", 11},
        {"clamp", 8}
    };

    const auto code = jit_compile_and_assembly(module, sizes, true);
    const auto clamp = code.code_start_as<Fn>("clamp").value();

    ASSERT_EQ(clamp(10, 0, 20), 10);
    ASSERT_EQ(clamp(10, 20, 30), 20);
    ASSERT_EQ(clamp(10, 5, 15), 10);
    ASSERT_EQ(clamp(10, 10, 10), 10);
    ASSERT_EQ(clamp(10, -5, 5), 5);
}

TEST(Clamp, clamp_f32) {
    const auto module = clamp_float(FloatingPointType::f32());
    verify_clamp_float<float(float, float, float)>(module);
}

TEST(Clamp, clamp_f64) {
    const auto module = clamp_float(FloatingPointType::f64());
    verify_clamp_float<double(double, double, double)>(module);
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}