#include <gtest/gtest.h>

#include <ir.h>

static Module ret_one() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "ret_one");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    data.ret(Value::i32(1));
    return builder.build();
}

TEST(EmptyFunc, traverse) {
    auto module = ret_one();
    const auto func = module.find_function_data("ret_one")
        .value();

    AnalysisPassCache cache;
    auto& preorder = *cache.analyze<PreorderTraverse>(func);
    ASSERT_EQ(preorder.size(), 1);

    ASSERT_EQ(preorder[0], func->first());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}