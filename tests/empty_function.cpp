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
    const auto& preorder = *cache.analyze<PreorderTraverse>(func);
    ASSERT_EQ(preorder.size(), 1);
    ASSERT_EQ(preorder[0], func->first());

    const auto& postorder = *cache.analyze<PostOrderTraverse>(func);
    ASSERT_EQ(postorder.size(), 1);
    ASSERT_EQ(postorder[0], func->first());

    const auto& bfs = *cache.analyze<BFSOrderTraverse>(func);
    ASSERT_EQ(bfs.size(), 1);
    ASSERT_EQ(bfs[0], func->first());

    const auto& linear_scan_order = *cache.analyze<LinearScanOrder>(func);
    ASSERT_EQ(linear_scan_order.size(), 1);
    ASSERT_EQ(linear_scan_order[0], func->first());
}

TEST(EmptyFunc, dom) {
    auto module = ret_one();
    const auto func = module.find_function_data("ret_one")
        .value();

    AnalysisPassCache cache;
    const auto& dom = *cache.analyze<DominatorTreeEval>(func);

    std::size_t size{};
    for (auto _: dom.dominators(func->first())) {
        size += 1;
    }

    ASSERT_EQ(size, 0);
}

TEST(LoopInfo, loop) {
    auto module = ret_one();
    const auto func = module.find_function_data("ret_one")
        .value();

    AnalysisPassCache cache;
    const auto& loop = *cache.analyze<LoopInfoEval>(func);
    ASSERT_EQ(loop.size(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}