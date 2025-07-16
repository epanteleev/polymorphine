#include <gtest/gtest.h>

#include "mir/mir.h"
#include "lir/x64/lir.h"

#include "lir/x64/asm/jit/JitAssembler.h"
#include "lir/x64/codegen/Codegen.h"

static JitCodeBlob do_jit_compilation(const Module& module, bool verbose = false) {
    Lowering lower(module);
    lower.run();
    const auto result = lower.result();
    if (verbose) {
        std::cout << result << std::endl;
    }

    Codegen codegen(result);
    codegen.run();
    const auto obj = codegen.result();
    if (verbose) {
        std::cout << obj << std::endl;
    }

    return JitAssembler::assembly(obj);
}

static Module ret_i32(const std::int32_t value) {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "ret_one");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    data.ret(Value::i32(value));
    return builder.build();
}

TEST(SanityCheck, ret_i32) {
    for (const auto i: {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN}) {
        const auto buffer = do_jit_compilation(ret_i32(i));
        const auto fn = reinterpret_cast<int(*)()>(buffer.code_start("ret_one").value());
        const auto res = fn();
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module ret_i64(const std::int64_t value) {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i64(), {}, "ret_one");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    data.ret(Value::i64(value));
    return builder.build();
}

TEST(SanityCheck, ret_i64) {
    for (const long i: {0L, 1L, -1L, 42L, -42L, 1000000L, -1000000L, INT64_MAX, INT64_MIN}) {
        const auto buffer = do_jit_compilation(ret_i64(i));
        const auto fn = reinterpret_cast<long(*)()>(buffer.code_start("ret_one").value());
        const auto res = fn();
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module ret_i8_u8(const std::int8_t value) {
    ModuleBuilder builder;
    {
        FunctionPrototype prototype(SignedIntegerType::i8(), {}, "ret_i8");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        const auto& data = *fn_builder.value();
        data.ret(Value::i8(value));
    }
    {
        FunctionPrototype prototype(UnsignedIntegerType::u8(), {}, "ret_u8");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        const auto& data = *fn_builder.value();
        data.ret(Value::u8(value));
    }

    return builder.build();
}

TEST(SanityCheck, ret_i8_u8) {
    for (const auto i: {0, 1, -1, 42, -42, 100, -100, INT8_MAX, INT8_MIN}) {
        const auto buffer = do_jit_compilation(ret_i8_u8(static_cast<std::int8_t>(i)));
        const auto fn_i8 = reinterpret_cast<std::int8_t(*)()>(buffer.code_start("ret_i8").value());
        const auto fn_u8 = reinterpret_cast<std::uint8_t(*)()>(buffer.code_start("ret_u8").value());
        const auto res_i8 = fn_i8();
        const auto res_u8 = fn_u8();
        ASSERT_EQ(res_i8, i) << "Failed for i8 value: " << i;
        ASSERT_EQ(res_u8, static_cast<std::uint8_t>(i)) << "Failed for u8 value: " << i;
    }
}

static Module ret_i32_arg() {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "ret_i32");
    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto arg0 = data.arg(0);
    data.ret(arg0);

    return builder.build();
}

TEST(SanityCheck, ret_i32_arg) {
    const auto buffer = do_jit_compilation(ret_i32_arg());
    const auto fn = reinterpret_cast<int(*)(int)>(buffer.code_start("ret_i32").value());
    for (const auto i: {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN}) {
        const auto res = fn(i);
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module add_i32_args(const NonTrivialType* ty) {
    ModuleBuilder builder;
    FunctionPrototype prototype(ty, {ty, ty}, "add");
    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto arg0 = data.arg(0);
    const auto arg1 = data.arg(1);
    const auto add = data.add(arg0, arg1);
    data.ret(add);

    return builder.build();
}

TEST(SanityCheck, add_i32_args) {
    const auto buffer = do_jit_compilation(add_i32_args(SignedIntegerType::i32()), true);
    const auto fn = reinterpret_cast<int(*)(int, int)>(buffer.code_start("add").value());

    std::vector values = {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN};
    for (const auto i: values) {
        for (const auto j: values) {
            const auto res = fn(i, j);
            ASSERT_EQ(res, i + j) << "Failed for values: " << i << ", " << j;
        }
    }
}

TEST(SanityCheck, add_i64_args) {
    const auto buffer = do_jit_compilation(add_i32_args(SignedIntegerType::i64()), true);
    const auto fn = reinterpret_cast<long(*)(long, long)>(buffer.code_start("add").value());

    std::vector<long> values = {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN, LONG_MAX, LONG_MIN};
    for (const auto i: values) {
        for (const auto j: values) {
            const auto res = fn(i, j);
            ASSERT_EQ(res, i + j) << "Failed for values: " << i << ", " << j;
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}