#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"
#include "lir/x64/lir.h"

static Module min_max_phi(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "min", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);

        const auto on_true = data.create_basic_block();
        const auto on_false = data.create_basic_block();
        const auto end = data.create_basic_block();
        const auto cond = data.icmp(IcmpPredicate::Lt, arg0, arg1);
        data.br_cond(cond, on_true, on_false);

        data.switch_block(on_true);
        data.br(end);

        data.switch_block(on_false);
        data.br(end);

        data.switch_block(end);
        const auto phi = data.phi(ty, {arg0, arg1}, {on_true, on_false});
        data.ret(phi);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "max", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);

        const auto on_true = data.create_basic_block();
        const auto on_false = data.create_basic_block();
        const auto end = data.create_basic_block();
        const auto cond = data.icmp(IcmpPredicate::Gt, arg0, arg1);
        data.br_cond(cond, on_true, on_false);

        data.switch_block(on_true);
        data.br(end);

        data.switch_block(on_false);
        data.br(end);

        data.switch_block(end);
        const auto phi = data.phi(ty, {arg0, arg1}, {on_true, on_false});
        data.ret(phi);
    }

    return builder.build();
}

TEST(SanityCheck2, min_max_phi_u8) {
    const auto buffer = jit_compile_and_assembly(min_max_phi(UnsignedIntegerType::u8()), true);
    const auto min = buffer.code_start_as<std::uint8_t(std::uint8_t, std::uint8_t)>("min").value();
    const auto max = buffer.code_start_as<std::uint8_t(std::uint8_t, std::uint8_t)>("max").value();

    ASSERT_EQ(min(-2, 3), 3);
    ASSERT_EQ(min(30, 20), 20);
    ASSERT_EQ(min(UINT8_MAX, 0), 0);
    ASSERT_EQ(min(UINT8_MAX, 0), 0);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(30, 30), 30);
    ASSERT_EQ(max(-1, 40), static_cast<std::uint8_t>(-1));
    ASSERT_EQ(min(UINT8_MAX, 0), 0);
    ASSERT_EQ(max(UINT8_MAX, 0), UINT8_MAX);
}

static Module min_max_phi_fp(const FloatingPointType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "min", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);

        const auto on_true = data.create_basic_block();
        const auto on_false = data.create_basic_block();
        const auto end = data.create_basic_block();
        const auto cond = data.fcmp(FcmpPredicate::Olt, arg0, arg1);
        data.br_cond(cond, on_true, on_false);

        data.switch_block(on_true);
        data.br(end);

        data.switch_block(on_false);
        data.br(end);

        data.switch_block(end);
        const auto phi = data.phi(ty, {arg0, arg1}, {on_true, on_false});
        data.ret(phi);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "max", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);

        const auto on_true = data.create_basic_block();
        const auto on_false = data.create_basic_block();
        const auto end = data.create_basic_block();
        const auto cond = data.fcmp(FcmpPredicate::Ogt, arg0, arg1);
        data.br_cond(cond, on_true, on_false);

        data.switch_block(on_true);
        data.br(end);

        data.switch_block(on_false);
        data.br(end);

        data.switch_block(end);
        const auto phi = data.phi(ty, {arg0, arg1}, {on_true, on_false});
        data.ret(phi);
    }

    return builder.build();
}

TEST(SanityCheck2, min_max_phi_f32) {
    const auto buffer = jit_compile_and_assembly(min_max_phi_fp(FloatingPointType::f32()), true);
    const auto min = buffer.code_start_as<float(float, float)>("min").value();
    const auto max = buffer.code_start_as<float(float, float)>("max").value();

    ASSERT_EQ(min(-2, 3), -2);
    ASSERT_EQ(min(30, 20), 20);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(-1, 40), 40);
    ASSERT_EQ(min(FLT_MAX, 0), 0);
    ASSERT_EQ(max(FLT_MAX, 0), FLT_MAX);
}

TEST(SanityCheck2, min_max_phi_f64) {
    const auto buffer = jit_compile_and_assembly(min_max_phi_fp(FloatingPointType::f64()), true);
    const auto min = buffer.code_start_as<double(double, double)>("min").value();
    const auto max = buffer.code_start_as<double(double, double)>("max").value();

    ASSERT_EQ(min(-2, 3), -2);
    ASSERT_EQ(min(30, 20), 20);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(-1, 40), 40);
    ASSERT_EQ(min(FLT_MAX, 0), 0);
    ASSERT_EQ(max(FLT_MAX, 0), FLT_MAX);
}

static Module xor_values(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "xor", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);

        const auto icmp = data.icmp(IcmpPredicate::Lt, arg0, arg1);
        const auto res = data.xxor(arg0, arg1);
        const auto select = data.select(icmp, Value::i32(-1), res);

        data.ret(select);
    }
    return builder.build();
}

template<std::integral T>
static T xor_values(T a, T b) noexcept {
    if (a < b) return -1;
    return a ^ b;
}

TEST(SanityCheck2, xor_values_i32) {
    const auto buffer = jit_compile_and_assembly(xor_values(SignedIntegerType::i32()));
    const auto xor_func = buffer.code_start_as<std::int32_t(std::int32_t, std::int32_t)>("xor").value();

    ASSERT_EQ(xor_func(10, 10), xor_values(10, 10));
    ASSERT_EQ(xor_func(10, 20), xor_values(10, 20));
    ASSERT_EQ(xor_func(-10, -20), xor_values(-10, -20));
    ASSERT_EQ(xor_func(0, 0), xor_values(0, 0));
}

static Module shl_values(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "shl", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);
        const auto res = data.shl(arg0, arg1);
        data.ret(res);
    }
    return builder.build();
}

static std::unordered_map<std::string, std::size_t> asm_sizes = {
    {"shl", 4},
};

TEST(SanityCheck2, shl_i8) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(SignedIntegerType::i8()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::int8_t(std::int8_t, std::int8_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), static_cast<std::int8_t>(1 << 1));
    ASSERT_EQ(shl_func(2, 3), static_cast<std::int8_t>(2 << 3));
    ASSERT_EQ(shl_func(0x1, 7), static_cast<std::int8_t>(0x1 << 7));
    ASSERT_EQ(shl_func(0xFF, 1), static_cast<std::int8_t>(0xFF << 1));
}

TEST(SanityCheck2, shl_u8) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(UnsignedIntegerType::u8()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::uint8_t(std::uint8_t, std::uint8_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), static_cast<std::uint8_t>(1 << 1));
    ASSERT_EQ(shl_func(2, 3), static_cast<std::uint8_t>(2 << 3));
    ASSERT_EQ(shl_func(0x1, 7), static_cast<std::uint8_t>(0x1 << 7));
    ASSERT_EQ(shl_func(0xFF, 1), static_cast<std::uint8_t>(0xFF << 1));
}

TEST(SanityCheck2, shl_i16) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(SignedIntegerType::i16()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::int16_t(std::int16_t, std::int16_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), static_cast<std::int16_t>(1 << 1));
    ASSERT_EQ(shl_func(2, 3), static_cast<std::int16_t>(2 << 3));
    ASSERT_EQ(shl_func(0x1, 15), static_cast<std::int16_t>(0x1 << 15));
    ASSERT_EQ(shl_func(0xFFFF, 1), static_cast<std::int16_t>(0xFFFF << 1));
}

TEST(SanityCheck2, shl_u16) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(UnsignedIntegerType::u16()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::uint16_t(std::uint16_t, std::uint16_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), static_cast<std::uint16_t>(1 << 1));
    ASSERT_EQ(shl_func(2, 3), static_cast<std::uint16_t>(2 << 3));
    ASSERT_EQ(shl_func(0x1, 15), static_cast<std::uint16_t>(0x1 << 15));
    ASSERT_EQ(shl_func(0xFFFF, 1), static_cast<std::uint16_t>(0xFFFF << 1));
}

TEST(SanityCheck2, shl_i32) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(SignedIntegerType::i32()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::int32_t(std::int32_t, std::int32_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), 1 << 1);
    ASSERT_EQ(shl_func(2, 3), 2 << 3);
    ASSERT_EQ(shl_func(0x1, 31), 0x1 << 31);
    ASSERT_EQ(shl_func(0xFFFFFFFF, 1), static_cast<std::int32_t>(0xFFFFFFFF << 1));
}

TEST(SanityCheck2, shl_u32) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(UnsignedIntegerType::u32()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::uint32_t(std::uint32_t, std::uint32_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), 1 << 1);
    ASSERT_EQ(shl_func(2, 3), 2 << 3);
    ASSERT_EQ(shl_func(0x1, 31), 0x1 << 31);
    ASSERT_EQ(shl_func(0xFFFFFFFF, 1), 0xFFFFFFFF << 1);
}

TEST(SanityCheck2, shl_i64) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(SignedIntegerType::i64()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::int64_t(std::int64_t, std::int64_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), 1 << 1);
    ASSERT_EQ(shl_func(2, 3), 2 << 3);
    ASSERT_EQ(shl_func(0x1, 63), 0x1LL << 63);
    ASSERT_EQ(shl_func(0xFFFFFFFFFFFFFFFF, 1), static_cast<std::int64_t>(0xFFFFFFFFFFFFFFFFLL << 1));
}

TEST(SanityCheck2, shl_u64) {
    const auto buffer = jit_compile_and_assembly({}, shl_values(UnsignedIntegerType::u64()), asm_sizes);
    const auto shl_func = buffer.code_start_as<std::uint64_t(std::uint64_t, std::uint64_t)>("shl").value();

    ASSERT_EQ(shl_func(1, 0), 1);
    ASSERT_EQ(shl_func(1, 1), 1 << 1);
    ASSERT_EQ(shl_func(2, 3), 2 << 3);
    ASSERT_EQ(shl_func(0x1, 63), 0x1UL << 63);
    ASSERT_EQ(shl_func(0xFFFFFFFFFFFFFFFF, 1), 0xFFFFFFFFFFFFFFFFUL << 1);
}

static Module shl_values_cst(const IntegerType* ty, const Value& count) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "shl", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto res = data.shl(arg0, count);
        data.ret(res);
    }
    return builder.build();
}

TEST(SanityCheck2, shl_u64_cst) {
    const std::vector<unsigned long> values = {0, 1, 3, 63};
    for (const auto value : values) {
        const auto buffer = jit_compile_and_assembly(shl_values_cst(UnsignedIntegerType::u64(), Value::u64(value)), true);
        const auto shl_func = buffer.code_start_as<std::uint64_t(std::uint64_t)>("shl").value();
        ASSERT_EQ(shl_func(1), 1UL<<value) << "value: " << value;
    }
}

TEST(SanityCheck2, shl_i64_cst) {
    const std::vector<long> values = {0, 1, 3, 63};
    for (const auto value :values) {
        const auto buffer = jit_compile_and_assembly(shl_values_cst(SignedIntegerType::i64(), Value::i64(value)), true);
        const auto shl_func = buffer.code_start_as<std::int64_t(std::int64_t)>("shl").value();
        ASSERT_EQ(shl_func(1), 1L<<value) << "value: " << value;
    }
}

static Module shr_values_cst(const IntegerType* ty, const Value& count) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "shr", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto res = data.shr(arg0, count);
        data.ret(res);
    }
    return builder.build();
}

TEST(SanityCheck2, shr_i64_cst) {
    const std::vector<long> values = {0, 1, 3, 63};
    for (const auto value: values) {
        const auto buffer = jit_compile_and_assembly(shr_values_cst(SignedIntegerType::i64(), Value::i64(value)), true);
        const auto shr_func = buffer.code_start_as<std::int64_t(std::int64_t)>("shr").value();
        ASSERT_EQ(shr_func(1L<<63), (1L<<63)>>value) << "value: " << value;
    }
}

TEST(SanityCheck2, shr_u64_cst) {
    const std::vector<unsigned long> values = {0, 1, 3, 63};
    for (auto value : values) {
        const auto buffer = jit_compile_and_assembly(shr_values_cst(UnsignedIntegerType::u64(), Value::u64(value)), true);
        const auto shr_func = buffer.code_start_as<std::uint64_t(std::uint64_t)>("shr").value();
        ASSERT_EQ(shr_func(1UL<<63), (1UL<<63)>>value) << "value: " << value;
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}