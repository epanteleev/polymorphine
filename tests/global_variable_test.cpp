#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

template<typename Fn>
static Module load_store_global_test(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto global = builder.add_variable("my_global_var", ty, 40L).value();
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "load_store", FunctionBind::INTERNAL);
        auto data = builder.make_function_builder(prototype).value();
        data.store(global, fn(47));
        data.ret(global);
    }
    return builder.build();
}

TEST(GlobalVariable, load_store_global_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"load_store", 3},
    };

    const auto module = load_store_global_test(SignedIntegerType::i64(), Value::i64);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t*()>("load_store").value();
    const auto result = fn();
    ASSERT_EQ(*result, 47);
    ASSERT_EQ(*result, 47); // Ensure the value is retained
}

TEST(GlobalVariable, load_store_global_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"load_store", 3},
    };

    const auto module = load_store_global_test(SignedIntegerType::i32(), Value::i32);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size, true);
    const auto fn = buffer.code_start_as<std::int32_t*()>("load_store").value();
    const auto result = fn();
    ASSERT_EQ(*result, 47);
    ASSERT_EQ(*result, 47); // Ensure the value is retained
}

TEST(GlobalVariable, load_store_global_i16) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"load_store", 3},
    };

    const auto module = load_store_global_test(SignedIntegerType::i16(), Value::i16);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size, true);
    const auto fn = buffer.code_start_as<std::int16_t*()>("load_store").value();
    const auto result = fn();
    ASSERT_EQ(*result, 47);
    ASSERT_EQ(*result, 47); // Ensure the value is retained
}

TEST(GlobalVariable, load_store_global_i8) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"load_store", 3},
    };

    const auto module = load_store_global_test(SignedIntegerType::i8(), Value::i8);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size, true);
    const auto fn = buffer.code_start_as<std::int8_t*()>("load_store").value();
    const auto result = fn();
    ASSERT_EQ(*result, 47);
    ASSERT_EQ(*result, 47); // Ensure the value is retained
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}