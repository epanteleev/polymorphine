#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"
#include "utility/Error.h"

static Module memcpy_test_module() {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(VoidType::type(), {PointerType::ptr(), PointerType::ptr(), UnsignedIntegerType::u64()}, "memcpy_test", FunctionVisibility::DEFAULT);

    auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();

    auto for_cond = data.create_basic_block();
    auto for_body = data.create_basic_block();
    auto for_inc = data.create_basic_block();
    auto for_end = data.create_basic_block();

    auto dst = data.arg(0);
    auto src = data.arg(1);
    auto n = data.arg(2);

    auto dst_addr = data.alloc(PointerType::ptr());
    auto src_addr = data.alloc(PointerType::ptr());
    auto n_addr = data.alloc(UnsignedIntegerType::u64());

    auto d = data.alloc(PointerType::ptr());
    auto s = data.alloc(PointerType::ptr());
    auto i = data.alloc(UnsignedIntegerType::u64());

    data.store(dst_addr, dst);
    data.store(src_addr, src);
    data.store(n_addr, n);

    auto v0 = data.load(PointerType::ptr(), dst_addr);
    data.store(d, v0);
    auto v1 = data.load(PointerType::ptr(), src_addr);
    data.store(s, v1);
    data.store(i, Value::u64(0));
    data.br(for_cond);

    data.switch_block(for_cond);
    auto v2 = data.load(UnsignedIntegerType::u64(), i);
    auto v3 = data.load(UnsignedIntegerType::u64(), n_addr);
    auto cmp = data.icmp(IcmpPredicate::Lt, v2, v3);
    data.br_cond(cmp, for_body, for_end);

    data.switch_block(for_body);
    auto v4 = data.load(PointerType::ptr(), s);
    auto v5 = data.load(UnsignedIntegerType::u64(), i);
    auto array_idx = data.gep(SignedIntegerType::i8(), v4, v5);
    auto v6 = data.load(SignedIntegerType::i8(), array_idx);
    auto v7 = data.load(PointerType::ptr(), d);
    auto v8 = data.load(UnsignedIntegerType::u64(), i);
    auto arrayidx1 = data.gep(SignedIntegerType::i8(), v7, v8);
    data.store(arrayidx1, v6);
    data.br(for_inc);

    data.switch_block(for_inc);
    auto v9 = data.load(UnsignedIntegerType::u64(), i);
    auto inc = data.add(v9, Value::u64(1));
    data.store(i, inc);
    data.br(for_cond);

    data.switch_block(for_end);
    data.ret();

    return builder.build();
}

TEST(Memcpy, basic) {
    char dst[10] = {};
    const char* src = "Hello";

    std::unordered_map<std::string, std::size_t> asm_size {
        {"memcpy_test", 27},
    };

    const auto func = jit_compile_and_assembly(memcpy_test_module(), asm_size, true);
    const auto memcpy_test = func.code_start_as<void(void*, const void*, std::size_t)>("memcpy_test").value();
    memcpy_test(dst, src, 6);
    ASSERT_STREQ(dst, "Hello");
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}