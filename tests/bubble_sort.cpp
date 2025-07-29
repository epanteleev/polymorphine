#include <gtest/gtest.h>

#include "mir/mir.h"
#include "helpers/Jit.h"

[[maybe_unused]]
static Module bubble_sort(const PrimitiveType* ty, const IntegerType* inc_type) {
    ModuleBuilder builder;
    FunctionPrototype prototype(VoidType::type(), {PointerType::ptr(), inc_type}, "bubble_sort");

    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    auto for_cond = data.create_basic_block();
    auto for_body = data.create_basic_block();
    auto for_cond1 = data.create_basic_block();
    auto for_body4 = data.create_basic_block();
    auto if_then = data.create_basic_block();
    auto if_end = data.create_basic_block();
    auto for_inc = data.create_basic_block();
    auto for_end = data.create_basic_block();
    auto for_inc18 = data.create_basic_block();
    auto for_end20 = data.create_basic_block();

    auto a_addr = data.alloc(PointerType::ptr());
    auto n_addr = data.alloc(ty);
    auto i = data.alloc(ty);
    auto j = data.alloc(ty);
    [[maybe_unused]]
    auto tmp = data.alloc(ty);
    auto a = data.arg(0);
    auto n = data.arg(1);

    data.store(a_addr, a);
    data.store(n_addr, n);
    data.store(i, Value::i32(0));
    data.store(j, Value::i32(0));
    data.br(for_cond);

    data.switch_block(for_cond);
    auto v0 = data.load(ty, i);
    auto v1 = data.load(ty, n_addr);
    auto cmp = data.icmp(IcmpPredicate::Lt, v0, v1);
    data.br_cond(cmp, for_body, for_end20);

    data.switch_block(for_body);
    data.store(j, Value::i32(0));
    data.br(for_cond1);

    data.switch_block(for_cond1);
    auto v2 = data.load(ty, j);
    auto v3 = data.load(ty, n_addr);
    auto v4 = data.load(ty, i);

    auto sub = data.sub(v3, v4);
    auto sub2 = data.sub(sub, Value::i32(1));
    auto cmp3 = data.icmp(IcmpPredicate::Lt, v2, sub2);
    data.br_cond(cmp3, for_body4, for_end);

    data.switch_block(for_body4);
    auto v5 = data.load(ty, a_addr);
    auto v6 = data.load(ty, j);
    [[maybe_unused]]
    auto idxprom = data.add(v5, v6);

    return builder.build();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}