#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "helpers/Utils.h"
#include "mir/mir.h"
#include "lir/x64/lir.h"

static Module min_max_phi(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        FunctionPrototype prototype(ty, {ty, ty}, "min");
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);

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
    {
        FunctionPrototype prototype(ty, {ty, ty}, "max");
        auto& data = *builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto arg1 = data.arg(1);

        const auto on_true = data.create_basic_block();
        const auto on_false = data.create_basic_block();
        const auto end = data.create_basic_block();
        const auto cond = data.icmp(IcmpPredicate::Gt, arg0, arg1);
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

TEST(SanityCheck2, min_max_phi_u8) {
    const auto buffer = jit_compile_and_assembly(min_max_phi(UnsignedIntegerType::u8()), true);
    const auto min = buffer.code_start_as<std::uint8_t(std::uint8_t, std::uint8_t)>("min").value();
    const auto max = buffer.code_start_as<std::uint8_t(std::uint8_t, std::uint8_t)>("max").value();

    ASSERT_EQ(min(-2, 3), 3);
    ASSERT_EQ(min(30, 20), 20);
    ASSERT_EQ(min(UINT8_MAX, 0), 0);
    ASSERT_EQ(min(UINT8_MAX, 0), 0);

    ASSERT_EQ(max(30, 20), 30);
    ASSERT_EQ(max(30, 30), 30);
    ASSERT_EQ(max(-1, 40), static_cast<std::uint8_t>(-1));
    ASSERT_EQ(max(UINT8_MAX, 0), UINT8_MAX);
    ASSERT_EQ(max(UINT8_MAX, 0), UINT8_MAX);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}