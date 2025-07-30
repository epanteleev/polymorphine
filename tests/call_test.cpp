#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module create_wrappers() {
    ModuleBuilder builder;

    {
        FunctionPrototype prototype(SignedIntegerType::i32(), {}, "call_test");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        auto& data = *fn_builder.value();
        const auto cont = data.create_basic_block();
        FunctionPrototype proto(SignedIntegerType::i32(), {}, "ret_42");
        const auto res = data.call(std::move(proto), cont, {});
        data.switch_block(cont);
        data.ret(res);
    }

    {
        FunctionPrototype ret_42(SignedIntegerType::i32(), {}, "ret_42");
        auto fn_builder = builder.make_function_builder(std::move(ret_42));
        auto& data = *fn_builder.value();
        data.ret(Value::i32(42));
    }

    return builder.build();
}


TEST(CallTest, CallFunction) {
    const auto module = create_wrappers();
    auto code = do_jit_compilation(module, true);
    const auto fn = code.code_start_as<std::int32_t()>("call_test").value();
    ASSERT_EQ(fn(), 42);
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}