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
        data.store(field1, Value::u64(100500));
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
    ASSERT_EQ(p.y, 100500);
    const auto sum = sum_fields(&p);
    ASSERT_EQ(sum, -42 + 100500);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}