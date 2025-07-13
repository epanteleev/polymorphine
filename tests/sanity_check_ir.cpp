#include <gtest/gtest.h>

#include "mir/mir.h"
#include "lir/x64/lir.h"

#include "../lib/lir/x64/asm/jit/JitAssembler.h"
#include "lir/x64/codegen/Codegen.h"
#include "lir/x64/codegen/MachFunctionCodegen.h"

Module ret_one() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "ret_one");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    data.ret(Value::i32(1));
    return builder.build();
}


TEST(SanityCheck, ret_one) {
    auto module = ret_one();
    AnalysisPassCacheMach cache;

    Lowering lower(module);
    lower.run();
    const auto result = lower.result();

    Codegen codegen(result);
    codegen.run();
    const auto obj_module = codegen.result();

    std::cout << obj_module;

    const auto buffer = JitAssembler::assembly(obj_module);

    std::cout << buffer;
    const auto fn = reinterpret_cast<int(*)()>(buffer.code_start("ret_one").value());
    const auto res = fn();
    ASSERT_EQ(res, 1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}