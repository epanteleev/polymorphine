#include <gtest/gtest.h>
#include <climits>

#include <mir/mir.h>
#include "helpers/Jit.h"

template<typename Fn>
static Module min(const PrimitiveType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {}, "min", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();

        const auto alloc1 = data.alloc(ty);
        data.store(alloc1, fn(2));

        const auto alloc2 = data.alloc(ty);
        data.store(alloc2, fn(3));


        const auto arg0 = data.load(ty, alloc1);
        const auto arg1 = data.load(ty, alloc2);

        const auto on_true = data.create_basic_block();
        const auto on_false = data.create_basic_block();
        const auto end = data.create_basic_block();
        const auto cond = data.icmp(IcmpPredicate::Lt, arg0, arg1);
        data.br_cond(cond, on_true, on_false);

        data.switch_block(on_true);
        data.br(end);

        data.switch_block(on_false);
        data.br(end);

        data.switch_block(end);
        const auto phi = data.phi(ty, {arg0, arg1}, {on_true, on_false});
        data.ret(phi);
    }

    return builder.build();
}

const std::unordered_map<std::string, std::size_t> external_symbols;
const std::unordered_map<std::string, std::size_t> asm_size = {
    {"cvt", 2}
};

TEST(MinFold, fold_all) {
    auto fib_mod = min(SignedIntegerType::i32(), Value::i32);
    OptPipeline pipeline;
    pipeline.add_pass<Mem2Reg>();
    pipeline.add_pass<Sccp>();

    const JitCompiler compiler(pipeline, true);
    const auto obj = compiler.compile(fib_mod);

    const auto fn = obj.code_start_as<std::int32_t()>("min").value();
    ASSERT_EQ(2, fn());
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}