#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module point_struct() {
    ModuleBuilder builder;
    auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
    {
        FunctionPrototype prototype(VoidType::type(), {PointerType::ptr()}, "init_point");
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(point_type, arg0, 0);
        const auto field1 = data.gfp(point_type, arg0, 1);
        data.store(field0, Value::i32(-42));
        data.store(field1, Value::u64(static_cast<std::int64_t>(INT_MAX)+2));
        data.ret();
    }
    {
        FunctionPrototype sum_fields_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "sum_fields");
        auto& data = *builder.make_function_builder(std::move(sum_fields_prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(point_type, arg0, 0);
        const auto field1 = data.gfp(point_type, arg0, 1);
        const auto val0 = data.load(SignedIntegerType::i32(), field0);
        const auto val1 = data.load(UnsignedIntegerType::u64(), field1);
        const auto val0_u64 = data.sext(SignedIntegerType::i64(), val0);
        const auto val1_i64 = data.bitcast(SignedIntegerType::i64(), val1);
        const auto sum = data.add(val0_u64, val1_i64);
        data.ret(sum);
    }

    return builder.build();
}

struct Point {
    int32_t x;
    uint64_t y;
};

TEST(StructAccess, init) {
    const auto buffer = jit_compile_and_assembly(point_struct(), true);
    const auto init = buffer.code_start_as<void(Point*)>("init_point").value();
    const auto sum_fields = buffer.code_start_as<int64_t(Point*)>("sum_fields").value();
    Point p{};
    init(&p);
    ASSERT_EQ(p.x, -42);
    ASSERT_EQ(p.y, static_cast<std::int64_t>(INT_MAX)+2);
    const auto sum = sum_fields(&p);
    ASSERT_EQ(sum, -42 + static_cast<std::int64_t>(INT_MAX)+2);
}

static Module vec_struct() {
    ModuleBuilder builder;
    auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
    auto vec_type = builder.add_struct_type("Vec", {point_type, point_type});
    {
        FunctionPrototype prototype(VoidType::type(), {PointerType::ptr()}, "init_vec");
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vec_type, arg0, 0);
        const auto field1 = data.gfp(vec_type, arg0, 1);
        const auto field0_0 = data.gfp(point_type, field0, 0);
        const auto field0_1 = data.gfp(point_type, field0, 1);
        const auto field1_0 = data.gfp(point_type, field1, 0);
        const auto field1_1 = data.gfp(point_type, field1, 1);
        data.store(field0_0, Value::i32(-42));
        data.store(field0_1, Value::u64(static_cast<std::int64_t>(INT_MAX)+2));
        data.store(field1_0, Value::i32(100));
        data.store(field1_1, Value::u64(static_cast<std::int64_t>(INT_MAX)+3));
        data.ret();
    }
    {
        FunctionPrototype sum_fields_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "sum_fields");
        auto& data = *builder.make_function_builder(std::move(sum_fields_prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vec_type, arg0, 0);
        const auto field1 = data.gfp(vec_type, arg0, 1);
        const auto field0_0 = data.gfp(point_type, field0, 0);
        const auto field0_1 = data.gfp(point_type, field0, 1);
        const auto field1_0 = data.gfp(point_type, field1, 0);
        const auto field1_1 = data.gfp(point_type, field1, 1);
        const auto val0_0 = data.load(SignedIntegerType::i32(), field0_0);
        const auto val0_1 = data.load(UnsignedIntegerType::u64(), field0_1);
        const auto val1_0 = data.load(SignedIntegerType::i32(), field1_0);
        const auto val1_1 = data.load(UnsignedIntegerType::u64(), field1_1);
        const auto val0_0_u64 = data.sext(SignedIntegerType::i64(), val0_0);
        const auto val0_1_i64 = data.bitcast(SignedIntegerType::i64(), val0_1);
        const auto val1_0_u64 = data.sext(SignedIntegerType::i64(), val1_0);
        const auto val1_1_i64 = data.bitcast(SignedIntegerType::i64(), val1_1);
        const auto sum = data.add(val0_0_u64, val1_0_u64);
        const auto sum2 = data.add(val0_1_i64, val1_1_i64);
        const auto total = data.add(sum, sum2);
        data.ret(total);
    }

    return builder.build();
}

struct Vec {
    Point p0;
    Point p1;
};

TEST(StructAccess, nested) {
    const auto buffer = jit_compile_and_assembly(vec_struct(), true);
    const auto init = buffer.code_start_as<void(Point*)>("init_vec").value();
    const auto sum_fields = buffer.code_start_as<int64_t(Point*)>("sum_fields").value();

    Vec v{};
    init(reinterpret_cast<Point*>(&v));
    ASSERT_EQ(v.p0.x, -42);
    ASSERT_EQ(v.p0.y, static_cast<std::int64_t>(INT_MAX)+2);
    ASSERT_EQ(v.p1.x, 100);
    ASSERT_EQ(v.p1.y, static_cast<std::int64_t>(INT_MAX)+3);
    const auto sum = sum_fields(reinterpret_cast<Point*>(&v));
    ASSERT_EQ(sum, -42 + static_cast<std::int64_t>(INT_MAX)+2 + 100 + static_cast<std::int64_t>(INT_MAX)+3);
}

static Module struct_stackalloc() {
    ModuleBuilder builder;
    auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
    {
        FunctionPrototype prototype(SignedIntegerType::i64(), {}, "make_point");
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto alloca = data.alloc(point_type);
        const auto field0 = data.gfp(point_type, alloca, 0);
        const auto field1 = data.gfp(point_type, alloca, 1);
        data.store(field0, Value::i32(-42));
        data.store(field1, Value::u64(static_cast<std::int64_t>(INT_MAX)+2));
        const auto val0 = data.load(SignedIntegerType::i32(), field0);
        const auto val1 = data.load(UnsignedIntegerType::u64(), field1);
        const auto val0_u64 = data.sext(SignedIntegerType::i64(), val0);
        const auto val1_i64 = data.bitcast(SignedIntegerType::i64(), val1);
        const auto sum = data.add(val0_u64, val1_i64);
        data.ret(sum);
    }

    return builder.build();
}

TEST(StructAlloc, stack) {
    const auto buffer = jit_compile_and_assembly(struct_stackalloc(), true);
    const auto make_point = buffer.code_start_as<int64_t()>("make_point").value();
    const auto sum = make_point();
    ASSERT_EQ(sum, static_cast<std::int64_t>(INT_MAX)-42+2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}