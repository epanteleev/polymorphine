#include <gtest/gtest.h>

#include "mir/mir.h"
#include "helpers/Jit.h"

template<typename Fn>
static Module bubble_sort(const PrimitiveType* ty, const PrimitiveType* inc_type, Fn&& inc_creator) {
    ModuleBuilder builder;
    FunctionPrototype prototype(VoidType::type(), {PointerType::ptr(), inc_type}, "bubble_sort", FunctionLinkage::DEFAULT);

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
    auto i = data.alloc(inc_type);
    auto j = data.alloc(inc_type);
    auto tmp = data.alloc(ty);
    auto a = data.arg(0);
    auto n = data.arg(1);

    data.store(a_addr, a);
    data.store(n_addr, n);
    data.store(i, inc_creator(0));
    data.store(j, inc_creator(0));
    data.br(for_cond);

    data.switch_block(for_cond);
    auto v0 = data.load(inc_type, i);
    auto v1 = data.load(ty, n_addr);
    auto cmp = data.icmp(IcmpPredicate::Lt, v0, v1);
    data.br_cond(cmp, for_body, for_end20);

    data.switch_block(for_body);
    data.store(j, inc_creator(0));
    data.br(for_cond1);

    data.switch_block(for_cond1);
    auto v2 = data.load(inc_type, j);
    auto v3 = data.load(ty, n_addr);
    auto v4 = data.load(inc_type, i);

    auto sub = data.sub(v3, v4);
    auto sub2 = data.sub(sub, inc_creator(1));
    auto cmp3 = data.icmp(IcmpPredicate::Lt, v2, sub2);
    data.br_cond(cmp3, for_body4, for_end);

    data.switch_block(for_body4);
    auto v5 = data.load(PointerType::ptr(), a_addr);
    auto v6 = data.load(inc_type, j);
    auto idxprom = data.sext(SignedIntegerType::i64(), v6);
    auto arrayidx = data.gep(ty, v5, idxprom);
    auto v7 = data.load(ty, arrayidx);
    auto v8 = data.load(PointerType::ptr(), a_addr);
    auto v9 = data.load(inc_type, j);
    auto add = data.add(v9, inc_creator(1));
    auto idxprom5 = data.sext(SignedIntegerType::i64(), add);
    auto arrayidx6 = data.gep(ty, v8, idxprom5);
    auto v10 = data.load(ty, arrayidx6);
    auto cmp7 = data.icmp(IcmpPredicate::Gt, v7, v10);
    data.br_cond(cmp7, if_then, if_end);

    data.switch_block(if_then);
    auto v11 = data.load(PointerType::ptr(), a_addr);
    auto v12 = data.load(inc_type, j);
    auto idxprom8 = data.sext(SignedIntegerType::i64(), v12);
    auto arrayidx9 = data.gep(ty, v11, idxprom8);
    auto v13 = data.load(ty, arrayidx9);
    data.store(tmp, v13);
    auto v14 = data.load(PointerType::ptr(), a_addr);
    auto v15 = data.load(inc_type, j);
    auto add10 = data.add(v15, inc_creator(1));
    auto idxprom11 = data.sext(SignedIntegerType::i64(), add10);
    auto arrayidx12 = data.gep(ty, v14, idxprom11);
    auto v16 = data.load(ty, arrayidx12);
    auto v17 = data.load(PointerType::ptr(), a_addr);
    auto v18 = data.load(inc_type, j);
    auto idxprom13 = data.sext(SignedIntegerType::i64(), v18);
    auto arrayidx14 = data.gep(ty, v17, idxprom13);
    data.store(arrayidx14, v16);
    auto v19 = data.load(ty, tmp);
    auto v20 = data.load(PointerType::ptr(), a_addr);
    auto v21 = data.load(inc_type, j);
    auto add15 = data.add(v21, inc_creator(1));
    auto idxprom16 = data.sext(SignedIntegerType::i64(), add15);
    auto arrayidx17 = data.gep(ty, v20, idxprom16);
    data.store(arrayidx17, v19);
    data.br(if_end);

    data.switch_block(if_end);
    data.br(for_inc);

    data.switch_block(for_inc);
    auto v22 = data.load(inc_type, j);
    auto inc = data.add(v22, inc_creator(1));
    data.store(j, inc);
    data.br(for_cond1);

    data.switch_block(for_end);
    data.br(for_inc18);

    data.switch_block(for_inc18);
    auto v23 = data.load(inc_type, i);
    auto inc19 = data.add(v23, Value::i32(1));
    data.store(i, inc19);
    data.br(for_cond);

    data.switch_block(for_end20);
    data.ret();

    return builder.build();
}

static std::unordered_map<std::string, std::size_t> symbol_sizes = {
    {"bubble_sort", 61}
};

template<std::integral T>
static bool is_sorted(const std::span<T>& arr) {
    for (std::size_t i = 1; i < arr.size(); ++i) {
        if (arr[i - 1] > arr[i]) {
            return false;
        }
    }
    return true;
}

TEST(BubbleSort, bubble_sort_i32) {
    const auto module = bubble_sort(SignedIntegerType::i32(), SignedIntegerType::i32(), Value::i32);
    const auto code = jit_compile_and_assembly({}, module, symbol_sizes, true);
    const auto fn = code.code_start_as<void(int*, int)>("bubble_sort").value();

    int arr[] = {5, 3, 8, 1, 2};
    fn(arr, 5);
    ASSERT_TRUE(is_sorted(std::span(arr, 5)));
}

TEST(BubbleSort, bubble_sort_i64) {
    const auto module = bubble_sort(SignedIntegerType::i64(), SignedIntegerType::i32(), Value::i32);
    const auto code = jit_compile_and_assembly({}, module, symbol_sizes, true);
    const auto fn = code.code_start_as<void(int64_t*, int64_t)>("bubble_sort").value();

    int64_t arr[] = {5, 3, 8, 1, 2};
    fn(arr, 5);
    ASSERT_TRUE(is_sorted(std::span(arr, 5)));
}

TEST(BubbleSort, bubble_sort_i16) {
    const auto module = bubble_sort(SignedIntegerType::i16(), SignedIntegerType::i8(), Value::i8);
    const auto code = jit_compile_and_assembly({}, module, symbol_sizes, true);
    const auto fn = code.code_start_as<void(int16_t*, int8_t)>("bubble_sort").value();

    int16_t arr[] = {5, 3, 8, 1, 2};
    fn(arr, 5);
    ASSERT_TRUE(is_sorted(std::span(arr, 5)));
}

TEST(BubbleSort, bubble_sort_u32) {
    const auto module = bubble_sort(UnsignedIntegerType::u64(), UnsignedIntegerType::u32(), Value::u32);
    const auto code = jit_compile_and_assembly({}, module, symbol_sizes, true);
    const auto fn = code.code_start_as<void(uint64_t*, uint32_t)>("bubble_sort").value();

    uint64_t arr[] = {5, 3, 8, 1, 2};
    fn(arr, 5);
    ASSERT_TRUE(is_sorted(std::span(arr, 5)));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}