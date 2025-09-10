#include <gtest/gtest.h>

#include <mir/mir.h>

static Module ret_constant() {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i32(), {}, "ret_one", FunctionLinkage::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();

    data.ret(Value::i32(1));
    return builder.build();
}

TEST(EmptyFunc, traverse) {
    auto module = ret_constant();
    const auto func = module.find_function_data("ret_one")
        .value();

    AnalysisPassManager cache;
    const auto& preorder = *cache.analyze<PreorderTraverse>(func);
    ASSERT_EQ(preorder.size(), 1);
    ASSERT_EQ(preorder[0], func->first());

    const auto& postorder = *cache.analyze<PostOrderTraverse>(func);
    ASSERT_EQ(postorder.size(), 1);
    ASSERT_EQ(postorder[0], func->first());

    const auto& bfs = *cache.analyze<BFSOrderTraverse>(func);
    ASSERT_EQ(bfs.size(), 1);
    ASSERT_EQ(bfs[0], func->first());
}

TEST(EmptyFunc, dom) {
    auto module = ret_constant();
    const auto func = module.find_function_data("ret_one")
        .value();

    AnalysisPassManager cache;
    const auto& dom = *cache.analyze<DominatorTreeEval>(func);

    std::size_t size{};
    for (auto _: dom.dominators(func->first())) {
        size += 1;
    }

    ASSERT_EQ(size, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}