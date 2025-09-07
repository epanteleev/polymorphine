#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module min_max_select(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "min", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);
        const auto cond = data.icmp(IcmpPredicate::Lt, arg0, arg1);
        const auto select = data.select(cond, arg0, arg1);
        data.ret(select);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "max", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);
        const auto cond = data.icmp(IcmpPredicate::Gt, arg0, arg1);
        const auto select = data.select(cond, arg0, arg1);
        data.ret(select);
    }

    return builder.build();
}

TEST(SanityCheck1, min_max_select_u8) {
    const auto buffer = jit_compile_and_assembly(min_max_select(UnsignedIntegerType::u8()), true);
    const auto min = buffer.code_start_as<std::uint8_t(std::uint8_t, std::uint8_t)>("min").value();
    const auto max = buffer.code_start_as<std::uint8_t(std::uint8_t, std::uint8_t)>("max").value();

    ASSERT_EQ(min(-2, 3), 3);
    ASSERT_EQ(min(30, 20), 20);
    ASSERT_EQ(min(UINT8_MAX, 0), 0);
    ASSERT_EQ(min(UINT8_MAX, 0), 0);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(30, 30), 30);
    ASSERT_EQ(max(-1, 40), static_cast<std::uint8_t>(-1));
    ASSERT_EQ(max(UINT8_MAX, 0), UINT8_MAX);
    ASSERT_EQ(max(UINT8_MAX, 0), UINT8_MAX);
}

TEST(SanityCheck1, min_max_select_u32) {
    const auto buffer = jit_compile_and_assembly(min_max_select(UnsignedIntegerType::u32()), true);
    const auto min = buffer.code_start_as<std::uint32_t(std::uint32_t, std::uint32_t)>("min").value();
    const auto max = buffer.code_start_as<std::uint32_t(std::uint32_t, std::uint32_t)>("max").value();

    ASSERT_EQ(min(-2, 3), 3);
    ASSERT_EQ(min(30, 20), 20);
    ASSERT_EQ(min(UINT_MAX, 0), 0);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(30, 30), 30);
    ASSERT_EQ(max(-30, 40), -30);
    ASSERT_EQ(max(UINT_MAX, 0), UINT_MAX);
}

TEST(SanityCheck1, min_max_select_i8) {
    const auto buffer = jit_compile_and_assembly(min_max_select(SignedIntegerType::i8()), true);
    const auto min = buffer.code_start_as<std::int8_t(std::int8_t, std::int8_t)>("min").value();
    const auto max = buffer.code_start_as<std::int8_t(std::int8_t, std::int8_t)>("max").value();

    ASSERT_EQ(min(-2, 3), -2);
    ASSERT_EQ(min(30, 20), 20);
    ASSERT_EQ(min(INT8_MAX, 0), 0);
    ASSERT_EQ(min(INT8_MAX, INT8_MIN), INT8_MIN);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(30, 30), 30);
    ASSERT_EQ(max(-30, 40), 40);
    ASSERT_EQ(max(INT8_MAX, 0), INT8_MAX);
    ASSERT_EQ(max(INT8_MAX, INT8_MIN), INT8_MAX);
}

TEST(SanityCheck1, min_max_select_i32) {
    const auto buffer = jit_compile_and_assembly(min_max_select(SignedIntegerType::i32()), true);
    const auto min = buffer.code_start_as<std::int32_t(std::int32_t, std::int32_t)>("min").value();
    const auto max = buffer.code_start_as<std::int32_t(std::int32_t, std::int32_t)>("max").value();

    ASSERT_EQ(min(-2, 3), -2);
    ASSERT_EQ(min(30, 20), 20);
    ASSERT_EQ(min(INT_MAX, 0), 0);
    ASSERT_EQ(min(INT_MAX, INT_MIN), INT_MIN);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(30, 30), 30);
    ASSERT_EQ(max(-30, 40), 40);
    ASSERT_EQ(max(INT_MAX, 0), INT_MAX);
    ASSERT_EQ(max(INT_MAX, INT_MIN), INT_MAX);
}

TEST(SanityCheck1, min_max_select_i64) {
    const auto buffer = jit_compile_and_assembly(min_max_select(SignedIntegerType::i64()), true);
    const auto min = buffer.code_start_as<std::int64_t(std::int64_t, std::int64_t)>("min").value();
    const auto max = buffer.code_start_as<std::int64_t(std::int64_t, std::int64_t)>("max").value();

    ASSERT_EQ(min(-2, 3), -2);
    ASSERT_EQ(min(30, 20), 20);
    ASSERT_EQ(min(LONG_MAX, 0), 0);
    ASSERT_EQ(min(LONG_MAX, LONG_MIN), LONG_MIN);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(30, 30), 30);
    ASSERT_EQ(max(-30, 40), 40);
    ASSERT_EQ(max(LONG_MAX, 0), LONG_MAX);
    ASSERT_EQ(max(LONG_MAX, LONG_MIN), LONG_MAX);
}

template<typename Fn>
static Module is_less_1(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_less_1", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto is_neg = data.icmp(IcmpPredicate::Lt, arg0, fn(1));
        const auto res = data.select(is_neg, Value::i8(1), Value::i8(0));
        data.ret(res);
    }
    return builder.build();
}

TEST(SanityCheck1, is_less_1_i8) {
    const auto buffer = jit_compile_and_assembly(is_less_1(SignedIntegerType::i8(), Value::i8), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::int8_t)>("is_less_1").value();

    for (const auto i: {INT8_MIN, -100, -1, 0, 1, 100, INT8_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::int8_t>(i));
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, is_less_1_u16) {
    const auto buffer = jit_compile_and_assembly(is_less_1(UnsignedIntegerType::u16(), Value::u16), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::uint16_t)>("is_less_1").value();

    for (const auto i: {0, 1, 100, UINT16_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::uint16_t>(i));
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, is_less_1_i32) {
    const auto buffer = jit_compile_and_assembly(is_less_1(SignedIntegerType::i32(), Value::i32), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::int32_t)>("is_less_1").value();

    for (const auto i: {INT32_MIN, -100, -1, 0, 1, 100, INT32_MAX}) {
        const auto res = is_less_1_fn(i);
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, is_less_1_u64) {
    const auto buffer = jit_compile_and_assembly(is_less_1(UnsignedIntegerType::u64(), Value::u64), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::uint64_t)>("is_less_1").value();

    for (const std::size_t i: {0UL, 1UL, 100UL, UINT64_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::uint64_t>(i));
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

template<typename Fn>
static Module is_neg2(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_less_1", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto is_neg = data.icmp(IcmpPredicate::Ge, arg0, fn(1));
        const auto res = data.select(is_neg, Value::i8(0), Value::i8(1));
        data.ret(res);
    }
    return builder.build();
}

TEST(SanityCheck1, is_neg2_i8) {
    const auto buffer = jit_compile_and_assembly(is_neg2(SignedIntegerType::i8(), Value::i8), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::int8_t)>("is_less_1").value();

    for (const auto i: {INT8_MIN, -100, -1, 0, 1, 100, INT8_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::int8_t>(i));
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, is_neg2_u16) {
    const auto buffer = jit_compile_and_assembly(is_neg2(UnsignedIntegerType::u16(), Value::u16), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::uint16_t)>("is_less_1").value();

    for (const auto i: {0, 1, 100, UINT16_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::uint16_t>(i));
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, is_neg2_i32) {
    const auto buffer = jit_compile_and_assembly(is_neg2(SignedIntegerType::i32(), Value::i32), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::int32_t)>("is_less_1").value();

    for (const auto i: {INT32_MIN, -100, -1, 0, 1, 100, INT32_MAX}) {
        const auto res = is_less_1_fn(i);
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, is_neg2_u64) {
    const auto buffer = jit_compile_and_assembly(is_neg2(UnsignedIntegerType::u64(), Value::u64), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::uint64_t)>("is_less_1").value();

    for (const std::size_t i: {0UL, 1UL, 100UL, UINT64_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::uint64_t>(i));
        ASSERT_EQ(res, i < 1 ? 1 : 0) << "Failed for value: " << i;
    }
}

template<typename Fn>
static Module is_less_1_2(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_less_1", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto is_neg = data.icmp(IcmpPredicate::Ge, arg0, fn(1));
        const auto res = data.select(is_neg, Value::i8(1), Value::i8(2));
        data.ret(res);
    }
    return builder.build();
}

TEST(SanityCheck1, select1_i8) {
    const auto buffer = jit_compile_and_assembly(is_less_1_2(SignedIntegerType::i8(), Value::i8));
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::int8_t)>("is_less_1").value();

    for (const auto i: {INT8_MIN, -100, -1, 0, 1, 100, INT8_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::int8_t>(i));
        ASSERT_EQ(res, i >= 1 ? 1 : 2) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, select1_u16) {
    const auto buffer = jit_compile_and_assembly(is_less_1_2(UnsignedIntegerType::u16(), Value::u16));
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::uint16_t)>("is_less_1").value();

    for (const auto i: {0, 1, 100, UINT16_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::uint16_t>(i));
        ASSERT_EQ(res, i >= 1 ? 1 : 2) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, select1_i32) {
    const auto buffer = jit_compile_and_assembly(is_less_1_2(SignedIntegerType::i32(), Value::i32), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::int32_t)>("is_less_1").value();

    for (const auto i: {INT32_MIN, -100, -1, 0, 1, 100, INT32_MAX}) {
        const auto res = is_less_1_fn(i);
        ASSERT_EQ(res, i >= 1 ? 1 : 2) << "Failed for value: " << i;
    }
}

TEST(SanityCheck1, select1_u64) {
    const auto buffer = jit_compile_and_assembly(is_less_1_2(UnsignedIntegerType::u64(), Value::u64), true);
    const auto is_less_1_fn = buffer.code_start_as<std::int8_t(std::uint64_t)>("is_less_1").value();

    for (const std::size_t i: {0UL, 1UL, 100UL, UINT64_MAX}) {
        const auto res = is_less_1_fn(static_cast<std::uint64_t>(i));
        ASSERT_EQ(res, i >= 1 ? 1 : 2) << "Failed for value: " << i;
    }
}

template<typename Fn>
static Module is_less_1_2_store(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(VoidType::type(), {PointerType::ptr(), ty}, "is_less_1", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
        const auto ptr = data.arg(0);
        const auto arg0 = data.arg(1);
        const auto is_neg = data.icmp(IcmpPredicate::Ge, arg0, fn(1));
        const auto res = data.select(is_neg, Value::i8(1), Value::i8(2));
        data.store(ptr, res);
        data.ret();
    }
    return builder.build();
}

TEST(SanityCheck1, is_less_1_store_i64) {
    const auto buffer = jit_compile_and_assembly(is_less_1_2_store(SignedIntegerType::i64(), Value::i64), true);
    const auto is_less_1_fn = buffer.code_start_as<void(std::int8_t*, std::int64_t)>("is_less_1").value();

    std::int8_t res{};
    for (const auto i: {INT64_MIN, -100L, -1L, 0L, 1L, 100L, INT64_MAX}) {
        is_less_1_fn(&res, i);
        ASSERT_EQ(res, i >= 1 ? 1 : 2) << "Failed for value: " << i;
    }
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}