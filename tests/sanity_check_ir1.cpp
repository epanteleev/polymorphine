#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"


static Module min_max_select(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        FunctionPrototype prototype(ty, {ty, ty}, "min");
        const auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);
        const auto cond = data.icmp(IcmpPredicate::Lt, arg0, arg1);
        const auto select = data.select(cond, arg0, arg1);
        data.ret(select);
    }
    {
        FunctionPrototype prototype(ty, {ty, ty}, "max");
        const auto& data = *builder.make_function_builder(std::move(prototype)).value();
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

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}