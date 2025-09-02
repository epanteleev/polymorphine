#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module struct_arg() {
    ModuleBuilder builder;
    {
        auto point_type = builder.add_struct_type("Vec", {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()});
        FunctionPrototype prototype(SignedIntegerType::i64(), {point_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}}, FunctionLinkage::DEFAULT);
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(point_type, arg0, 0);
        const auto field1 = data.gfp(point_type, arg0, 1);
        const auto field2 = data.gfp(point_type, arg0, 2);
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

TEST(StructArg, pass_by_value) {
    const auto buffer = jit_compile_and_assembly(struct_arg(), true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec)>("sum_fields").value();
    Vec v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(v);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}