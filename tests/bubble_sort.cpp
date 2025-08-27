#include <gtest/gtest.h>

#include "mir/mir.h"
#include "helpers/Jit.h"

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
    auto idxprom = data.sext(SignedIntegerType::i64(), v6);
    auto arrayidx = data.gep(ty, v5, idxprom);
    auto v7 = data.load(ty, arrayidx);
    auto v8 = data.load(ty, a_addr);
    auto v9 = data.load(ty, j);
    auto add = data.add(v9, Value::i32(1));
    auto idxprom5 = data.sext(SignedIntegerType::i64(), add);
    auto arrayidx6 = data.gep(ty, v8, idxprom5);
    auto v10 = data.load(ty, arrayidx6);
    auto cmp7 = data.icmp(IcmpPredicate::Gt, v7, v10);
    data.br_cond(cmp7, if_then, if_end);

    data.switch_block(if_then);
    auto v11 = data.load(PointerType::ptr(), a_addr);
    auto v12 = data.load(ty, j);
    auto idxprom8 = data.sext(SignedIntegerType::i64(), v12);
    auto arrayidx9 = data.gep(ty, v11, idxprom8);
    auto v13 = data.load(ty, arrayidx9);
    data.store(tmp, v13);
    auto v14 = data.load(PointerType::ptr(), a_addr);
    auto v15 = data.load(ty, j);
    auto add10 = data.add(v15, Value::i32(1));
    auto idxprom11 = data.sext(SignedIntegerType::i64(), add10);
    auto arrayidx12 = data.gep(ty, v14, idxprom11);
    auto v16 = data.load(ty, arrayidx12);
    auto v17 = data.load(PointerType::ptr(), a_addr);
    auto v18 = data.load(ty, j);
    auto idxprom13 = data.sext(SignedIntegerType::i64(), v18);
    auto arrayidx14 = data.gep(ty, v17, idxprom13);
    data.store(arrayidx14, v16);
    auto v19 = data.load(PointerType::ptr(), tmp);
    auto v20 = data.load(PointerType::ptr(), a_addr);
    auto v21 = data.load(ty, j);
    auto add15 = data.add(v21, Value::i32(1));
    auto idxprom16 = data.sext(SignedIntegerType::i64(), add15);
    auto arrayidx17 = data.gep(ty, v20, idxprom16);
    data.store(arrayidx17, v19);
    data.br(if_end);

    data.switch_block(if_end);
    data.br(for_inc);

    data.switch_block(for_inc);
    auto v22 = data.load(ty, j);
    auto inc = data.add(v22, Value::i32(1));
    data.store(j, inc);
    data.br(for_cond1);

    data.switch_block(for_end);
    data.br(for_inc18);

    data.switch_block(for_inc18);
    auto v23 = data.load(ty, i);
    auto inc19 = data.add(v23, Value::i32(1));
    data.store(i, inc19);
    data.br(for_cond);

    data.switch_block(for_end20);
    data.ret();

    return builder.build();
}

TEST(BubbleSort, bubble_sort_i32) {
    GTEST_SKIP();
    const auto module = bubble_sort(SignedIntegerType::i32(), SignedIntegerType::i32());
    const auto code = jit_compile_and_assembly({}, module, {{"bubble_sort", 100}}, true);
    const auto fn = code.code_start_as<void(int*, int)>("bubble_sort").value();

    int arr[] = {5, 3, 8, 1, 2};
    fn(arr, 5);
    ASSERT_EQ(arr[0], 1);
    ASSERT_EQ(arr[1], 2);
    ASSERT_EQ(arr[2], 3);
    ASSERT_EQ(arr[3], 5);
    ASSERT_EQ(arr[4], 8);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}