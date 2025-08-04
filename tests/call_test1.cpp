#include <gtest/gtest.h>

#include "utility/Error.h"

#include "helpers/Jit.h"
#include "mir/mir.h"


int return_constant() {
    printf("hereh");
    return 34;
}

JitCodeBlob do_compile_and_assembly(const std::unordered_map<const aasm::Symbol*, std::size_t>& external_symbols, const ObjModule& module, const bool verbose) {
    const auto buffer = JitAssembler::assembly(external_symbols, module);
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}

TEST(CallTest, call_external) {
    GTEST_SKIP();
    ModuleBuilder builder;
    {
        FunctionPrototype prototype(SignedIntegerType::i32(), {}, "call_external");
        auto fn_builder = builder.make_function_builder(std::move(prototype));
        auto& data = *fn_builder.value();
        const auto cont = data.create_basic_block();
        FunctionPrototype proto(SignedIntegerType::i32(), {}, "return_constant", FunctionLinkage::EXTERN);
        const auto res = data.call(std::move(proto), cont, {});
        data.switch_block(cont);
        data.ret(res);
    }

    const auto module = builder.build();
    auto obj = do_jit_compile(module, true);
    const auto ret_constant = obj.symbol_table()->add("return_constant", aasm::Linkage::EXTERNAL);

    std::unordered_map<const aasm::Symbol*, std::size_t> external_symbols = {
        {ret_constant.first, reinterpret_cast<std::size_t>(&return_constant)}
    };
    const auto code = do_compile_and_assembly(external_symbols, obj, true);
    const auto fn = code.code_start_as<std::int32_t()>("call_external").value();
    ASSERT_EQ(fn(), 34);

}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
