#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module global_constant_test(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {}, "global_constant_test", FunctionVisibility::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", ty, 42).value();
        const auto value = data.load(ty, constant);
        data.ret(value);
    }
    return builder.build();
}

TEST(GlobalConstant, load_global_constant_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_constant_test", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, global_constant_test(SignedIntegerType::i64()), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("global_constant_test").value();
    ASSERT_EQ(fn(), 42);
}

TEST(GlobalConstant, load_global_constant_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
            {"global_constant_test", 2},
        };

    const auto buffer = jit_compile_and_assembly({}, global_constant_test(SignedIntegerType::i32()), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("global_constant_test").value();
    ASSERT_EQ(fn(), 42);
}

static Module global_constant_test_string() {
    ModuleBuilder builder;
    {
        const auto array_type = builder.add_array_type(SignedIntegerType::i8(), 8);
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "global_constant_test", FunctionVisibility::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", array_type, "success").value();
        data.ret(constant);
    }
    return builder.build();
}

TEST(GlobalConstant, get_string) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_constant_test", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, global_constant_test_string(), asm_size, true);
    const auto fn = buffer.code_start_as<char*()>("global_constant_test").value();
    ASSERT_STREQ(fn(), "success");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}