#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

template<typename Fn>
static Module load_store_global_test(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto global = builder.add_variable("my_global_var", ty, 40L).value();
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "load_store", FunctionBind::INTERNAL);
        const auto data = builder.make_function_builder(prototype).value();
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

static Module global_struct_variable_test() {
    ModuleBuilder builder;
    {
        const auto struct_type = builder.add_struct_type("MyStruct", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        const auto global = builder.add_variable("my_global_var", struct_type, Initializer{1L, 2L}).value();
        const auto prototype = builder.add_function_prototype(struct_type, {}, "load_store", FunctionBind::INTERNAL);
        const auto data = builder.make_function_builder(prototype).value();
        const auto field_ptr = data.gfp(struct_type, global, 1);
        data.store(field_ptr, Value::i64(84));
        data.ret(global);
    }
    return builder.build();
}

struct MyStruct {
    std::int64_t a;
    std::int64_t b;
};

TEST(GlobalVariable, load_store_global_struct) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"load_store", 3},
    };

    const auto module = global_struct_variable_test();
    const auto buffer = jit_compile_and_assembly({}, module, asm_size, true);
    const auto fn = buffer.code_start_as<MyStruct*()>("load_store").value();
    const auto result = fn();
    ASSERT_EQ(result->a, 1);
    ASSERT_EQ(result->b, 84);
    ASSERT_EQ(result->b, 84); // Ensure the value is retained
}

static Module global_array_test(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto array_type = builder.add_array_type(ty, 100);
        const auto global_array = builder.add_variable("array", array_type, Initializer{1L, 2L}).value();
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "global_array", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        data.ret(global_array);
    }

    return builder.build();
}

TEST(GlobalVariable, escape_array_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_array", 2}
    };
    const auto module = global_array_test(SignedIntegerType::i32());
    const auto buffer = jit_compile_and_assembly({}, module, asm_size);
    const auto fn = buffer.code_start_as<std::int32_t*()>("global_array").value();
    const auto result = fn();
    ASSERT_EQ(result[0], 1);
    ASSERT_EQ(result[1], 2);
    for (auto i = 3; i < 100; i++) {
        ASSERT_EQ(result[i], 0);
    }
}

TEST(GlobalVariable, escape_array_u64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_array", 2}
    };
    const auto module = global_array_test(UnsignedIntegerType::u64());
    const auto buffer = jit_compile_and_assembly({}, module, asm_size);
    const auto fn = buffer.code_start_as<std::uint64_t*()>("global_array").value();
    const auto result = fn();
    ASSERT_EQ(result[0], 1);
    ASSERT_EQ(result[1], 2);
    for (auto i = 3; i < 100; i++) {
        ASSERT_EQ(result[i], 0);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}