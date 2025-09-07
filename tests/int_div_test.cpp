#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module idiv(const SignedIntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "idiv", FunctionLinkage::DEFAULT);
        const auto& data = *builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto b = data.arg(1);
        const auto c = data.idiv(a, b);
        data.ret(c.first);
    }
    return builder.build();
}

TEST(Idiv, basic) {
    GTEST_SKIP();
    const auto buffer = jit_compile_and_assembly(idiv(SignedIntegerType::i64()), true);
    const auto idiv = buffer.code_start_as<std::int64_t(std::int64_t, std::int64_t)>("make_point2").value();
    const auto p = idiv(4, 2);
    ASSERT_EQ(p, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}