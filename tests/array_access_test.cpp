#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module create_array() {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "create_array", FunctionLinkage::DEFAULT);
        auto data = builder.make_function_builder(std::move(prototype)).value();
        const auto arr_type = builder.add_array_type(SignedIntegerType::i64(), 5);
        const auto alloca = data.alloc(arr_type);
        for (std::int64_t i = 0; i < 5; ++i) {
            auto gep = data.gep(SignedIntegerType::i64(), alloca, Value::i64(i));
            data.store(gep, Value::i64(i * 10));
        }
        auto sum = data.alloc(SignedIntegerType::i64());
        data.store(sum, Value::i64(0));
        for (std::int64_t i = 0; i < 5; ++i) {
            auto gep = data.gep(SignedIntegerType::i64(), alloca, Value::i64(i));
            auto val = data.load(SignedIntegerType::i64(), gep);
            auto current_sum = data.load(SignedIntegerType::i64(), sum);
            auto new_sum = data.add(current_sum, val);
            data.store(sum, new_sum);
        }
        auto final_sum = data.load(SignedIntegerType::i64(), sum);
        data.ret(final_sum);
    }

    return builder.build();
}

TEST(ArrayAccess, basic) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"create_array", 32},
    };
    const auto buffer = jit_compile_and_assembly({}, create_array(), asm_size, true);
    const auto create_array_fn = buffer.code_start_as<std::int64_t()>("create_array").value();
    const auto result = create_array_fn();
    ASSERT_EQ(result, 0 + 10 + 20 + 30 + 40);
}

static Module create_array_of_structs() {
    ModuleBuilder builder;
    {
        auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
        const auto arr_type = builder.add_array_type(point_type, 3);

        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "create_array_of_points", FunctionLinkage::DEFAULT);
        auto data = builder.make_function_builder(std::move(prototype)).value();

        const auto alloca = data.alloc(arr_type);
        for (std::int32_t i{}; i < 3; ++i) {
            auto gep = data.gep(point_type, alloca, Value::i64(i));
            auto field0 = data.gfp(point_type, gep, 0);
            auto field1 = data.gfp(point_type, gep, 1);
            data.store(field0, Value::i32(i * 10));
            data.store(field1, Value::u64(i * 100));
        }
        auto sum = data.alloc(SignedIntegerType::i64());
        data.store(sum, Value::i64(0));
        for (std::int64_t i{}; i < 3; ++i) {
            auto gep = data.gep(point_type, alloca, Value::i64(i));
            auto field0 = data.gfp(point_type, gep, 0);
            auto field1 = data.gfp(point_type, gep, 1);
            auto val0 = data.load(SignedIntegerType::i32(), field0);
            auto val1 = data.load(UnsignedIntegerType::u64(), field1);
            auto val0_i64 = data.sext(SignedIntegerType::i64(), val0);
            auto val1_i64 = data.bitcast(SignedIntegerType::i64(), val1);
            auto current_sum = data.load(SignedIntegerType::i64(), sum);
            auto new_sum = data.add(current_sum, val0_i64);
            new_sum = data.add(new_sum, val1_i64);
            data.store(sum, new_sum);
        }
        auto final_sum = data.load(SignedIntegerType::i64(), sum);
        data.ret(final_sum);
    }

    return builder.build();
}

TEST(ArrayAccess, array_of_structs) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"create_array_of_points",  18},
    };

    const auto buffer = jit_compile_and_assembly(create_array_of_structs(), true);
    const auto create_array_fn = buffer.code_start_as<std::int64_t()>("create_array_of_points").value();
    const auto result = create_array_fn();
    ASSERT_EQ(result, (0 + 0) + (10 + 100) + (20 + 200));
}

static Module create_struct_of_arrays() {
    ModuleBuilder builder;
    {
        auto array_type = builder.add_array_type(SignedIntegerType::i32(), 3);
        auto array_holder_type = builder.add_struct_type("ArrayHolder", {SignedIntegerType::i64(), array_type});
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "create_struct_of_arrays", FunctionLinkage::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto alloca = data.alloc(array_holder_type);
        for (std::int32_t i{}; i < 3; ++i) {
            auto field1 = data.gfp(array_holder_type, alloca, 1);
            auto gep = data.gep(SignedIntegerType::i32(), field1, Value::i64(i));
            data.store(gep, Value::i32(i * 10));
        }

        auto field0 = data.gfp(array_holder_type, alloca, 0);
        data.store(field0, Value::i64(42));
        auto sum = data.alloc(SignedIntegerType::i64());
        data.store(sum, Value::i64(0));

        for (std::int64_t i{}; i < 3; ++i) {
            auto field1 = data.gfp(array_holder_type, alloca, 1);
            auto gep = data.gep(SignedIntegerType::i32(), field1, Value::i64(i));
            auto val = data.load(SignedIntegerType::i32(), gep);
            auto val_i64 = data.sext(SignedIntegerType::i64(), val);
            auto current_sum = data.load(SignedIntegerType::i64(), sum);
            auto new_sum = data.add(current_sum, val_i64);
            data.store(sum, new_sum);
        }
        auto final_sum = data.load(SignedIntegerType::i64(), sum);
        data.ret(final_sum);
    }

    return builder.build();
}

TEST(ArrayAccess, struct_of_arrays) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"create_struct_of_arrays", 26},
    };

    const auto buffer = jit_compile_and_assembly({}, create_struct_of_arrays(), asm_size, true);
    const auto create_struct_fn = buffer.code_start_as<std::int64_t()>("create_struct_of_arrays").value();
    const auto result = create_struct_fn();
    ASSERT_EQ(result, 10 + 20);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}