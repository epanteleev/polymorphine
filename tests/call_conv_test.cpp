#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module struct_arg_size1() {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()});
        FunctionPrototype prototype(SignedIntegerType::i64(), {vect_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}}, FunctionLinkage::DEFAULT);
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(SignedIntegerType::i64(), field0);
        const auto val1 = data.load(SignedIntegerType::i64(), field1);
        const auto val2 = data.load(SignedIntegerType::i64(), field2);
        const auto sum = data.add(data.add(val0, val1), val2);
        data.ret(sum);
    }

    return builder.build();
}
struct Vec {
    int64_t x;
    int64_t y;
    int64_t z;
};

TEST(StructArg, pass_one_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 6},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size1(), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec)>("sum_fields").value();
    Vec v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(v);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100);
}

static Module struct_arg_size2() {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()});
        FunctionPrototype prototype(SignedIntegerType::i64(), {vect_type, vect_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}}, FunctionLinkage::DEFAULT);
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(SignedIntegerType::i64(), field0);
        const auto val1 = data.load(SignedIntegerType::i64(), field1);
        const auto val2 = data.load(SignedIntegerType::i64(), field2);
        const auto sum = data.add(data.add(val0, val1), val2);
        const auto arg1 = data.arg(1);
        const auto field3 = data.gfp(vect_type, arg1, 0);
        const auto field4 = data.gfp(vect_type, arg1, 1);
        const auto field5 = data.gfp(vect_type, arg1, 2);
        const auto val3 = data.load(SignedIntegerType::i64(), field3);
        const auto val4 = data.load(SignedIntegerType::i64(), field4);
        const auto val5 = data.load(SignedIntegerType::i64(), field5);
        const auto sum2 = data.add(data.add(val3, val4), val5);
        const auto total = data.add(sum, sum2);
        data.ret(total);
    }

    return builder.build();
}

TEST(StructArg, pass_two_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 12},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size2(), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec, Vec)>("sum_fields").value();
    Vec v1{};
    v1.x = -42;
    v1.y = static_cast<std::int64_t>(INT_MAX)+2;
    v1.z = 100;
    Vec v2{};
    v2.x = 1;
    v2.y = 2;
    v2.z = 3;
    const auto sum = sum_fields(v1, v2);
    ASSERT_EQ(sum, (static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100) + (1 + 2 + 3));
}

static Module struct_arg_size3() {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()});
        auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        FunctionPrototype prototype(SignedIntegerType::i64(), {vect_type, vect_type, point_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}}, FunctionLinkage::DEFAULT);
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(SignedIntegerType::i64(), field0);
        const auto val1 = data.load(SignedIntegerType::i64(), field1);
        const auto val2 = data.load(SignedIntegerType::i64(), field2);
        const auto sum = data.add(data.add(val0, val1), val2);

        const auto arg1 = data.arg(1);
        const auto field3 = data.gfp(vect_type, arg1, 0);
        const auto field4 = data.gfp(vect_type, arg1, 1);
        const auto field5 = data.gfp(vect_type, arg1, 2);
        const auto val3 = data.load(SignedIntegerType::i64(), field3);
        const auto val4 = data.load(SignedIntegerType::i64(), field4);
        const auto val5 = data.load(SignedIntegerType::i64(), field5);
        const auto sum2 = data.add(data.add(val3, val4), val5);

        const auto arg2 = data.arg(2);
        const auto field6 = data.gfp(point_type, arg2, 0);
        const auto field7 = data.gfp(point_type, arg2, 1);
        const auto val6 = data.load(SignedIntegerType::i64(), field6);
        const auto val7 = data.load(SignedIntegerType::i64(), field7);
        const auto sum3 = data.add(val6, val7);
        const auto total = data.add(data.add(sum, sum2), sum3);
        data.ret(total);
    }
    return builder.build();
}

struct Point {
    std::int64_t x;
    std::int64_t y;
    std::int64_t z;
    std::int64_t w;
};

TEST(StructArg, pass_three_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 16},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size3(), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec, Vec, Point)>("sum_fields").value();
    Vec v1{};
    v1.x = -42;
    v1.y = static_cast<std::int64_t>(INT_MAX)+2;
    v1.z = 100;
    Vec v2{};
    v2.x = 1;
    v2.y = 2;
    v2.z = 3;
    Point v3{};
    v3.x = 10;
    v3.y = 20;
    const auto sum = sum_fields(v1, v2, v3);
    ASSERT_EQ(sum, (static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100) + (1 + 2 + 3) + (10 + 20));
}

static Module array_of_structs_with_cst() {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()});
        FunctionPrototype prototype(SignedIntegerType::i64(), {vect_type, SignedIntegerType::i64()}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{}}, FunctionLinkage::DEFAULT);
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(SignedIntegerType::i64(), field0);
        const auto val1 = data.load(SignedIntegerType::i64(), field1);
        const auto val2 = data.load(SignedIntegerType::i64(), field2);
        const auto arg1 = data.arg(1);
        const auto sum = data.add(data.add(val0, val1), data.add(arg1, val2));
        data.ret(sum);
    }

    return builder.build();
}

TEST(StructArg, array_of_structs_with_cst) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 7},
    };

    const auto buffer = jit_compile_and_assembly({}, array_of_structs_with_cst(), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec, std::int64_t)>("sum_fields").value();
    Vec v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(v, 5);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100 + 5);
}

static Module array_of_structs_with_cst0() {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()});
        FunctionPrototype prototype(SignedIntegerType::i64(), {SignedIntegerType::i64(), vect_type}, "sum_fields", std::vector{AttributeSet{}, AttributeSet{Attribute::ByValue}}, FunctionLinkage::DEFAULT);
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(1);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(SignedIntegerType::i64(), field0);
        const auto val1 = data.load(SignedIntegerType::i64(), field1);
        const auto val2 = data.load(SignedIntegerType::i64(), field2);
        const auto arg1 = data.arg(0);
        const auto sum = data.add(data.add(val0, val1), data.add(arg1, val2));
        data.ret(sum);
    }

    return builder.build();
}

TEST(StructArg, array_of_structs_with_cst0) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 7},
    };

    const auto buffer = jit_compile_and_assembly({}, array_of_structs_with_cst0(), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(std::int64_t, Vec)>("sum_fields").value();
    Vec v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(5, v);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100 + 5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}