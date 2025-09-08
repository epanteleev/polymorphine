#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module idiv(const SignedIntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "idiv", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
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

    const auto buffer = jit_compile_and_assembly({}, idiv(SignedIntegerType::i64()), asm_size, true);
    const auto idiv = buffer.code_start_as<std::int64_t(std::int64_t, std::int64_t)>("idiv").value();
    const auto p = idiv(4, 2);
    ASSERT_EQ(p, 2);
}

TEST(Idiv, basic_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv", 4},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv(SignedIntegerType::i32()), asm_size, true);
    const auto idiv = buffer.code_start_as<std::int32_t(std::int32_t, std::int32_t)>("idiv").value();
    const auto p = idiv(9, 3);
    ASSERT_EQ(p, 3);
}

template<typename Fn>
static Module idiv_2(const SignedIntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "idiv_2", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
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

    const auto buffer = jit_compile_and_assembly({}, idiv_2(SignedIntegerType::i64(), Value::i64), asm_size, true);
    const auto idiv = buffer.code_start_as<std::int64_t(std::int64_t)>("idiv_2").value();
    const auto p = idiv(5);
    ASSERT_EQ(p, 2);
}

TEST(Idiv, idiv_to_2_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv_2", 5},
    };

    const auto buffer = jit_compile_and_assembly({}, idiv_2(SignedIntegerType::i32(), Value::i32), asm_size, true);
    const auto idiv = buffer.code_start_as<std::int32_t(std::int32_t)>("idiv_2").value();
    const auto p = idiv(5);
    ASSERT_EQ(p, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}