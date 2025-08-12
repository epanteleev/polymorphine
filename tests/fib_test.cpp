#include <gtest/gtest.h>

#include "mir/mir.h"
#include "helpers/Jit.h"

template<typename Fn>
static Module fib(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    FunctionPrototype prototype(ty, {ty}, "fib");

    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    auto n = data.arg(0);
    auto ret_addr = data.alloc(ty);
    auto n_addr = data.alloc(ty);

    auto a = data.alloc(ty);
    auto b = data.alloc(ty);
    auto c = data.alloc(ty);
    auto i = data.alloc(ty);

    data.store(n_addr, n);
    data.store(a, fn(0));
    data.store(b, fn(1));

    auto v0 = data.load(ty, n_addr);
    auto cmp0 = data.icmp(IcmpPredicate::Eq, v0, fn(0));

    auto if_then = data.create_basic_block();
    auto if_end = data.create_basic_block();
    auto for_cond = data.create_basic_block();
    auto for_body = data.create_basic_block();
    auto for_inc = data.create_basic_block();
    auto for_end = data.create_basic_block();
    auto ret = data.create_basic_block();

    data.br_cond(cmp0, if_then, if_end);

    data.switch_block(if_then);

    auto v1 = data.load(ty, a);
    data.store(ret_addr, v1);
    data.br(ret);

    data.switch_block(if_end);
    data.store(i, fn(2));
    data.br(for_cond);

    data.switch_block(for_cond);
    auto v2 = data.load(ty, i);
    auto v3 = data.load(ty, n_addr);

    auto cmp = data.icmp(IcmpPredicate::Le, v2, v3);
    data.br_cond(cmp, for_body, for_end);

    data.switch_block(for_body);
    auto v4 = data.load(ty, a);
    auto v5 = data.load(ty, b);
    auto add = data.add(v4, v5);
    data.store(c, add);
    auto v6 = data.load(ty, b);
    data.store(a, v6);
    auto v7 = data.load(ty, c);
    data.store(b, v7);
    data.br(for_inc);

    data.switch_block(for_inc);

    auto v8 = data.load(ty, i);
    auto inc = data.add(v8, fn(1));
    data.store(i, inc);

    data.br(for_cond);

    data.switch_block(for_end);
    auto v9 = data.load(ty, b);
    data.store(ret_addr, v9);
    data.br(ret);

    data.switch_block(ret);
    auto v10 = data.load(ty, ret_addr);

    data.ret(v10);
    return builder.build();
}

template<std::integral T>
static T fib_value(T n) {
    T n0 = 0;
    T n1 = 1;
    if (n == 0) return n0;
    if (n == 1) return n1;
    for (T i = 2; i <= n; ++i) {
        const T n2 = n0 + n1;
        n0 = n1;
        n1 = n2;
    }
    return n1;
}

static const std::unordered_map<std::string, std::size_t> fib_sizes = {
    {"fib", 37}
};

TEST(Fib, i8) {
    const auto buffer = jit_compile_and_assembly(fib(SignedIntegerType::i8(), Value::i8), fib_sizes, false);
    const auto fn = buffer.code_start_as<std::int8_t(std::int8_t)>("fib").value();

    for (std::int8_t i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << static_cast<int>(i);
    }
}

TEST(Fib, u8) {
    const auto buffer = jit_compile_and_assembly(fib(UnsignedIntegerType::u8(), Value::u8), fib_sizes, false);
    const auto fn = buffer.code_start_as<std::uint8_t(std::uint8_t)>("fib").value();

    for (std::uint8_t i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << static_cast<int>(i);
    }
}

TEST(Fib, i16) {
    const auto buffer = jit_compile_and_assembly(fib(SignedIntegerType::i16(), Value::i16));
    const auto fn = buffer.code_start_as<std::int16_t(std::int16_t)>("fib").value();

    for (std::int16_t i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << static_cast<int>(i);
    }
}

TEST(Fib, u16) {
    const auto buffer = jit_compile_and_assembly(fib(UnsignedIntegerType::u16(), Value::u16), fib_sizes, false);
    const auto fn = buffer.code_start_as<std::uint16_t(std::uint16_t)>("fib").value();

    for (std::uint16_t i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << static_cast<int>(i);
    }
}

TEST(Fib, u32) {
    const auto buffer = jit_compile_and_assembly(fib(UnsignedIntegerType::u32(), Value::u32), fib_sizes, false);
    const auto fn = buffer.code_start_as<std::uint32_t(std::uint32_t)>("fib").value();

    for (std::uint32_t i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << i;
    }
}

TEST(Fib, i32) {
    const auto buffer = jit_compile_and_assembly(fib(SignedIntegerType::i32(), Value::i32), fib_sizes, false);
    const auto fn = buffer.code_start_as<int(int)>("fib").value();

    for (int i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << i;
    }
}

TEST(Fib, i64) {
    const auto buffer = jit_compile_and_assembly(fib(SignedIntegerType::i64(), Value::i64), fib_sizes, false);
    const auto fn = buffer.code_start_as<long(long)>("fib").value();

    for (long i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << i;
    }
}

TEST(Fib, u64) {
    const auto buffer = jit_compile_and_assembly( fib(UnsignedIntegerType::u64(), Value::u64), fib_sizes, true);
    const auto fn = buffer.code_start_as<std::uint64_t(std::uint64_t)>("fib").value();

    for (std::uint64_t i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << i;
    }
}

static Module recursive_fib(const IntegerType* ty) {
    ModuleBuilder builder;
    FunctionPrototype prototype(ty, {ty}, "fib_recursive");
    auto copy1 = prototype;
    auto copy2 = prototype;

    auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    auto n = data.arg(0);
    auto ret_addr = data.alloc(ty);

    auto base_case = data.icmp(IcmpPredicate::Le, n, Value::i64(1));
    auto if_then = data.create_basic_block();
    auto if_end = data.create_basic_block();
    auto if_else = data.create_basic_block();
    data.br_cond(base_case, if_then, if_else);

    data.switch_block(if_then);
    data.store(ret_addr, n);
    data.br(if_end);

    data.switch_block(if_else);
    auto sub = data.sub(n, Value::i64(1));
    auto cont = data.create_basic_block();
    auto call1 = data.call(std::move(copy1), cont, {sub});

    data.switch_block(cont);
    auto sub2 = data.sub(n, Value::i64(2));
    auto cont1 = data.create_basic_block();
    auto call2 = data.call(std::move(copy2), cont1, {sub2});
    data.switch_block(cont1);

    auto add = data.add(call1, call2);
    data.store(ret_addr, add);
    data.br(if_end);
    data.switch_block(if_end);

    auto ret_val = data.load(ty, ret_addr);
    data.ret(ret_val);

    return builder.build();
}

TEST(Fib, RecursiveFib) {
    const std::unordered_map<std::string, size_t> sizes = {
        {"fib_recursive", 28}
    };
    const auto buffer = jit_compile_and_assembly(sizes, recursive_fib(SignedIntegerType::i64()), true);
    const auto fn = buffer.code_start_as<long(long)>("fib_recursive").value();

    for (long i = 0; i < 20; ++i) {
        const auto res = fn(i);
        ASSERT_EQ(res, fib_value(i)) << "Failed for value: " << i;
    }
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}