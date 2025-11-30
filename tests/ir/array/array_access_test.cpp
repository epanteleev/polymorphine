#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

template<typename V>
static Module create_array(const PrimitiveType* ty, V&& val_fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {}, "create_array", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arr_type = builder.add_array_type(ty, 5);
        const auto alloca = data.alloc(arr_type);
        for (std::int64_t i{}; i < 5; ++i) {
            auto gep = data.gep(ty, alloca, Value::i64(i));
            data.store(gep, val_fn(i * 10));
        }
        auto sum = data.alloc(ty);
        data.store(sum, val_fn(0));
        for (std::int64_t i{}; i < 5; ++i) {
            auto gep = data.gep(ty, alloca, Value::i64(i));
            auto val = data.load(ty, gep);
            auto current_sum = data.load(ty, sum);
            auto new_sum = data.add(current_sum, val);
            data.store(sum, new_sum);
        }
        const auto final_sum = data.load(ty, sum);
        data.ret(final_sum);
    }

    return builder.build();
}

TEST(ArrayAccess, basic) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"create_array", 27},
    };
    const auto buffer = jit_compile_and_assembly({}, create_array(SignedIntegerType::i64(), Value::i64), asm_size, true);
    const auto create_array_fn = buffer.code_start_as<std::int64_t()>("create_array").value();
    const auto result = create_array_fn();
    ASSERT_EQ(result, 0 + 10 + 20 + 30 + 40);
}

TEST(ArrayAccess, basic_f32) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"create_array", 33},
    };
    const auto buffer = jit_compile_and_assembly({}, create_array(FloatingPointType::f32(), Value::f32), asm_size, true);
    const auto create_array_fn = buffer.code_start_as<float()>("create_array").value();
    const auto result = create_array_fn();
    ASSERT_EQ(result, 0 + 10 + 20 + 30 + 40);
}

TEST(ArrayAccess, basic_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"create_array", 33},
    };
    const auto buffer = jit_compile_and_assembly({}, create_array(FloatingPointType::f64(), Value::f64), asm_size, true);
    const auto create_array_fn = buffer.code_start_as<double()>("create_array").value();
    const auto result = create_array_fn();
    ASSERT_EQ(result, 0 + 10 + 20 + 30 + 40);
}

static Module create_array_of_structs() {
    ModuleBuilder builder;
    {
        auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
        const auto arr_type = builder.add_array_type(point_type, 3);

        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "create_array_of_points", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();

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
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "create_struct_of_arrays", FunctionBind::DEFAULT);
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
        {"create_struct_of_arrays", 23},
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