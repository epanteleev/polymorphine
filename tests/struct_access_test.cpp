#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module init_struct() {
    ModuleBuilder builder;
    {
        auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
        FunctionPrototype prototype(VoidType::type(), {PointerType::ptr()}, "init_point");
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(point_type, arg0, Value::i64(0));
        const auto field1 = data.gfp(point_type, arg0, Value::i64(1));
        data.store(field0, Value::i32(-42));
        data.store(field1, Value::u64(100500));
        data.ret();
    }
    return builder.build();
}

struct Point {
    int32_t x;
    uint64_t y;
};

TEST(StructAccess, Init) {
    GTEST_SKIP();
    const auto buffer = jit_compile_and_assembly(init_struct(), true);
    const auto init = buffer.code_start_as<void(Point*)>("init").value();
    Point p{};
    init(&p);
    ASSERT_EQ(p.x, -42);
    ASSERT_EQ(p.y, 100500);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}