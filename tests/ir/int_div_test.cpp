#include <gtest/gtest.h>

#include "../helpers/Jit.h"
#include "mir/mir.h"

static Module idiv(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "idiv", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto b = data.arg(1);
        const auto c = data.idiv(a, b);
        data.ret(c.first);
    }
    return builder.build();
}

TEST(Idiv, basic_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv(SignedIntegerType::i64()), asm_size);
    const auto idiv = buffer.code_start_as<std::int64_t(std::int64_t, std::int64_t)>("idiv").value();
    const auto p = idiv(4, 2);
    ASSERT_EQ(p, 2);
}

TEST(Idiv, basic_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv(SignedIntegerType::i32()), asm_size);
    const auto idiv = buffer.code_start_as<std::int32_t(std::int32_t, std::int32_t)>("idiv").value();
    const auto p = idiv(9, 3);
    ASSERT_EQ(p, 3);
}

TEST(Idiv, basic_u64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv(UnsignedIntegerType::u64()), asm_size);
    const auto idiv = buffer.code_start_as<std::uint64_t(std::uint64_t, std::uint64_t)>("idiv").value();
    const auto p = idiv(4, 2);
    ASSERT_EQ(p, 2);
}

TEST(Idiv, basic_u32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv(UnsignedIntegerType::u32()), asm_size);
    const auto idiv = buffer.code_start_as<std::uint32_t(std::uint32_t, std::uint32_t)>("idiv").value();
    const auto p = idiv(9, 3);
    ASSERT_EQ(p, 3);
}

template<typename Fn>
static Module idiv_2(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "idiv_2", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto c = data.idiv(a, fn(2));
        data.ret(c.first);
    }
    return builder.build();
}

TEST(Idiv, idiv_to_2_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_2", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_2(SignedIntegerType::i64(), Value::i64), asm_size);
    const auto idiv = buffer.code_start_as<std::int64_t(std::int64_t)>("idiv_2").value();
    const auto p = idiv(5);
    ASSERT_EQ(p, 2);
}

TEST(Idiv, idiv_to_2_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_2", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_2(SignedIntegerType::i32(), Value::i32), asm_size);
    const auto idiv = buffer.code_start_as<std::int32_t(std::int32_t)>("idiv_2").value();
    const auto p = idiv(5);
    ASSERT_EQ(p, 2);
}

TEST(Idiv, idiv_to_2_u64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_2", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_2(UnsignedIntegerType::u64(), Value::u64), asm_size);
    const auto idiv = buffer.code_start_as<std::uint64_t(std::uint64_t)>("idiv_2").value();
    const auto p = idiv(5);
    ASSERT_EQ(p, 2);
}

TEST(Idiv, idiv_to_2_u32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_2", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_2(UnsignedIntegerType::u32(), Value::u32), asm_size, true);
    const auto idiv = buffer.code_start_as<std::uint32_t(std::uint32_t)>("idiv_2").value();
    const auto p = idiv(5);
    ASSERT_EQ(p, 2);
}

static Module reminder(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "reminder", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto b = data.arg(1);
        const auto c = data.idiv(a, b);
        data.ret(c.second);
    }
    return builder.build();
}

TEST(Idiv, reminder_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"reminder", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, reminder(SignedIntegerType::i64()), asm_size);
    const auto reminder = buffer.code_start_as<std::int64_t(std::int64_t, std::int64_t)>("reminder").value();
    const auto p = reminder(5, 2);
    ASSERT_EQ(p, 1);
}

TEST(Idiv, reminder_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"reminder", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, reminder(SignedIntegerType::i32()), asm_size);
    const auto reminder = buffer.code_start_as<std::int32_t(std::int32_t, std::int32_t)>("reminder").value();
    const auto p = reminder(5, 2);
    ASSERT_EQ(p, 1);
}

TEST(Idiv, reminder_u64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"reminder", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, reminder(UnsignedIntegerType::u64()), asm_size);
    const auto reminder = buffer.code_start_as<std::uint64_t(std::uint64_t, std::uint64_t)>("reminder").value();
    const auto p = reminder(5, 2);
    ASSERT_EQ(p, 1);
}

TEST(Idiv, reminder_u32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"reminder", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, reminder(UnsignedIntegerType::u32()), asm_size);
    const auto reminder = buffer.code_start_as<std::uint32_t(std::uint32_t, std::uint32_t)>("reminder").value();
    const auto p = reminder(5, 2);
    ASSERT_EQ(p, 1);
}

struct DivI64 {
    std::int64_t quotient;
    std::int64_t remainder;
};

static Module idiv_quotient_reminder_i64(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto tuple_ty = TupleType::tuple(ty, ty);
        const auto prototype = builder.add_function_prototype(tuple_ty, {ty, ty}, "idiv_quotient_reminder_i64", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto b = data.arg(1);
        const auto c = data.idiv(a, b);
        data.ret(c.first, c.second);
    }
    return builder.build();
}

TEST(Idiv, idiv_quotient_reminder_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_quotient_reminder_i64", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_quotient_reminder_i64(SignedIntegerType::i64()), asm_size);
    const auto idiv = buffer.code_start_as<DivI64(std::int64_t, std::int64_t)>("idiv_quotient_reminder_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 2);
    ASSERT_EQ(p.remainder, 1);
}

TEST(Idiv, idiv_quotient_reminder_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_quotient_reminder_i64", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_quotient_reminder_i64(SignedIntegerType::i32()), asm_size);
    const auto idiv = buffer.code_start_as<DivI64(std::int32_t, std::int32_t)>("idiv_quotient_reminder_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 2);
    ASSERT_EQ(p.remainder, 1);
}

TEST(Idiv, idiv_quotient_reminder_u64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_quotient_reminder_i64", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_quotient_reminder_i64(UnsignedIntegerType::u64()), asm_size);
    const auto idiv = buffer.code_start_as<DivI64(std::uint64_t, std::uint64_t)>("idiv_quotient_reminder_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 2);
    ASSERT_EQ(p.remainder, 1);
}

TEST(Idiv, idiv_quotient_reminder_u32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_quotient_reminder_i64", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_quotient_reminder_i64(UnsignedIntegerType::u32()), asm_size);
    const auto idiv = buffer.code_start_as<DivI64(std::uint32_t, std::uint32_t)>("idiv_quotient_reminder_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 2);
    ASSERT_EQ(p.remainder, 1);
}

static Module idiv_reminder_quotient_i64(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto tuple_ty = TupleType::tuple(ty, ty);
        const auto prototype = builder.add_function_prototype(tuple_ty, {ty, ty}, "idiv_reminder_quotient_i64", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto b = data.arg(1);
        const auto c = data.idiv(a, b);
        data.ret(c.second, c.first);
    }
    return builder.build();
}

TEST(Idiv, idiv_reminder_quotient_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_reminder_quotient_i64", 7},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_reminder_quotient_i64(SignedIntegerType::i64()), asm_size);
    const auto idiv = buffer.code_start_as<DivI64(std::int64_t, std::int64_t)>("idiv_reminder_quotient_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 1);
    ASSERT_EQ(p.remainder, 2);
}

TEST(Idiv, idiv_reminder_quotient_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_reminder_quotient_i64", 7}, // TODO better lowering
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_reminder_quotient_i64(SignedIntegerType::i32()), asm_size, true);
    const auto idiv = buffer.code_start_as<DivI64(std::int32_t, std::int32_t)>("idiv_reminder_quotient_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 1);
    ASSERT_EQ(p.remainder, 2);
}

TEST(Idiv, idiv_reminder_quotient_u64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_reminder_quotient_i64", 7}, // TODO better lowering
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_reminder_quotient_i64(UnsignedIntegerType::u64()), asm_size, true);
    const auto idiv = buffer.code_start_as<DivI64(std::uint64_t, std::uint64_t)>("idiv_reminder_quotient_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 1);
    ASSERT_EQ(p.remainder, 2);
}

TEST(Idiv, idiv_reminder_quotient_u32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_reminder_quotient_i64", 7}, // TODO better lowering
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_reminder_quotient_i64(UnsignedIntegerType::u32()), asm_size, true);
    const auto idiv = buffer.code_start_as<DivI64(std::uint32_t, std::uint32_t)>("idiv_reminder_quotient_i64").value();
    const auto p = idiv(5, 2);
    ASSERT_EQ(p.quotient, 1);
    ASSERT_EQ(p.remainder, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}