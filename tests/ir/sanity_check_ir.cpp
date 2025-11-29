#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "helpers/Utils.h"
#include "mir/mir.h"
#include "lir/x64/lir.h"

static Module ret_i32(const std::int32_t value) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i32(), {}, "ret_one", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();

    data.ret(Value::i32(value));
    return builder.build();
}

TEST(SanityCheck, ret_i32) {
    for (const auto i: {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN}) {
        const auto buffer = jit_compile_and_assembly(ret_i32(i));
        const auto fn = buffer.code_start_as<std::int32_t()>("ret_one").value();
        const auto res = fn();
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module ret_i64(const std::int64_t value) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "ret_one", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();

    data.ret(Value::i64(value));
    return builder.build();
}

TEST(SanityCheck, ret_i64) {
    for (const long i: {0L, 1L, -1L, 42L, -42L, 1000000L, -1000000L, INT64_MAX, INT64_MIN}) {
        const auto buffer = jit_compile_and_assembly(ret_i64(i));
        const auto fn = buffer.code_start_as<std::int64_t()>("ret_one").value();
        const auto res = fn();
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module ret_f32(const FloatingPointType* fp, const Value& value) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(fp, {}, "ret_one", FunctionBind::DEFAULT);

    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();

    data.ret(value);
    return builder.build();
}

TEST(SanityCheck, ret_f32) {
    for (const double i: {0., 1., -1., 42., -42., 1000000., -1000000., static_cast<double>(INT64_MAX), static_cast<double>(INT64_MIN), static_cast<double>(FLT_MAX), static_cast<double>(FLT_MIN)}) {
        const auto buffer = jit_compile_and_assembly(ret_f32(FloatingPointType::f32(), Value::f32(i)), true);
        const auto fn = buffer.code_start_as<float()>("ret_one").value();
        const auto res = fn();
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

TEST(SanityCheck, ret_f64) {
    for (const double i: {0., 1., -1., 42., -42., 1000000., -1000000., static_cast<double>(INT64_MAX), static_cast<double>(INT64_MIN), DBL_MAX, DBL_MIN}) {
        const auto buffer = jit_compile_and_assembly(ret_f32(FloatingPointType::f64(), Value::f64(i)), true);
        const auto fn = buffer.code_start_as<double()>("ret_one").value();
        const auto res = fn();
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module ret_i8_u8(const std::int8_t value) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i8(), {}, "ret_i8", FunctionBind::DEFAULT);
        const auto fn_builder = builder.make_function_builder(prototype);
        const auto data = fn_builder.value();
        data.ret(Value::i8(value));
    }
    {
        const auto prototype = builder.add_function_prototype(UnsignedIntegerType::u8(), {}, "ret_u8", FunctionBind::DEFAULT);
        const auto fn_builder = builder.make_function_builder(prototype);
        const auto data = fn_builder.value();
        data.ret(Value::u8(value));
    }

    return builder.build();
}

TEST(SanityCheck, ret_i8_u8) {
    for (const auto i: {0, 1, -1, 42, -42, 100, -100, INT8_MAX, INT8_MIN}) {
        const auto buffer = jit_compile_and_assembly(ret_i8_u8(static_cast<std::int8_t>(i)));
        const auto fn_i8 = buffer.code_start_as<std::int8_t()>("ret_i8").value();
        const auto fn_u8 = buffer.code_start_as<std::uint8_t()>("ret_u8").value();
        const auto res_i8 = fn_i8();
        const auto res_u8 = fn_u8();
        ASSERT_EQ(res_i8, i) << "Failed for i8 value: " << i;
        ASSERT_EQ(res_u8, static_cast<std::uint8_t>(i)) << "Failed for u8 value: " << i;
    }
}

static Module ret_prim_arg(const PrimitiveType* ty) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(ty, {ty}, "ret_i32", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();
    const auto arg0 = data.arg(0);
    data.ret(arg0);

    return builder.build();
}

TEST(SanityCheck, ret_i32_arg) {
    const auto buffer = jit_compile_and_assembly(ret_prim_arg(SignedIntegerType::i32()));
    const auto fn = buffer.code_start_as<int(int)>("ret_i32").value();
    for (const auto i: {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN}) {
        const auto res = fn(i);
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

TEST(SanityCheck, ret_f32_arg) {
    const auto buffer = jit_compile_and_assembly(ret_prim_arg(FloatingPointType::f32()));
    const auto fn = buffer.code_start_as<float(float)>("ret_i32").value();
    for (const double i: {0., 1., -1., 42., -42., 1000000., -1000000., static_cast<double>(INT32_MAX), static_cast<double>(INT32_MIN)}) {
        const auto res = fn(i);
        ASSERT_EQ(res, static_cast<float>(i)) << "Failed for value: " << i;
    }
}

TEST(SanityCheck, ret_f64_arg) {
    const auto buffer = jit_compile_and_assembly(ret_prim_arg(FloatingPointType::f64()));
    const auto fn = buffer.code_start_as<double(double)>("ret_i32").value();
    for (const double i: {0., 1., -1., 42., -42., 1000000., -1000000., static_cast<double>(INT32_MAX), static_cast<double>(INT32_MIN)}) {
        const auto res = fn(i);
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module add_prim_args(const NonTrivialType* ty) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "add", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();
    const auto arg0 = data.arg(0);
    const auto arg1 = data.arg(1);
    const auto add = data.add(arg0, arg1);
    data.ret(add);

    return builder.build();
}

TEST(SanityCheck, add_i32_args) {
    const auto buffer = jit_compile_and_assembly(add_prim_args(SignedIntegerType::i32()));
    const auto fn = buffer.code_start_as<int(int, int)>("add").value();

    std::vector values = {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN};
    for (const auto i: values) {
        for (const auto j: values) {
            const auto res = fn(i, j);
            ASSERT_EQ(res, add_overflow(i, j)) << "Failed for values: " << i << ", " << j;
        }
    }
}

static const std::unordered_map<std::string, std::size_t> asm_size = {
    {"add", 2},
};

TEST(SanityCheck, add_f32_args) {
    const auto buffer = jit_compile_and_assembly({}, add_prim_args(FloatingPointType::f32()), asm_size, true);
    const auto fn = buffer.code_start_as<float(float, float)>("add").value();

    std::vector values = {0., 1., -1., 42., -42., 1000000., -1000000., static_cast<double>(INT32_MAX), static_cast<double>(INT32_MIN)};
    for (const auto i: values) {
        for (const auto j: values) {
            const auto res = fn(i, j);
            ASSERT_EQ(res, static_cast<float>(i) + static_cast<float>(j)) << "Failed for values: " << i << ", " << j;
        }
    }
}

TEST(SanityCheck, add_f64_args) {
    const auto buffer = jit_compile_and_assembly({}, add_prim_args(FloatingPointType::f64()), asm_size, true);
    const auto fn = buffer.code_start_as<double(double, double)>("add").value();

    std::vector values = {0., 1., -1., 42., -42., 1000000., -1000000., static_cast<double>(INT32_MAX), static_cast<double>(INT32_MIN)};
    for (const auto i: values) {
        for (const auto j: values) {
            const auto res = fn(i, j);
            ASSERT_EQ(res, i + j) << "Failed for values: " << i << ", " << j;
        }
    }
}

TEST(SanityCheck, add_i64_args) {
    const auto buffer = jit_compile_and_assembly(add_prim_args(SignedIntegerType::i64()));
    const auto fn = buffer.code_start_as<long(long, long)>("add").value();

    std::vector<long> values = {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN, LONG_MAX, LONG_MIN};
    for (const auto i: values) {
        for (const auto j: values) {
            const auto res = fn(i, j);
            ASSERT_EQ(res, add_overflow(i, j)) << "Failed for values: " << i << ", " << j;
        }
    }
}

static Module branch() {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(SignedIntegerType::i32(), {}, "ret", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();

    auto* cont = data.create_basic_block();
    data.br(cont);
    data.switch_block(cont);
    data.ret(Value::i32(10));

    return builder.build();
}

TEST(SanityCheck, branch1) {
    const auto buffer = jit_compile_and_assembly(branch(), true);
    std::cout << buffer << std::endl;
    const auto fn = buffer.code_start_as<int()>("ret").value();
    const auto res = fn();
    ASSERT_EQ(res, 10) << "Failed for value: " << 0;
}

static Module stack_alloc(const Value& val) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(val.type(), {}, "stackalloc", FunctionBind::DEFAULT);
    const auto fn_builder = builder.make_function_builder(prototype);
    auto data = fn_builder.value();
    const auto alloc = data.alloc(val.as_type<PrimitiveType>());
    data.store(alloc, val);
    data.ret(data.load(val.as_type<PrimitiveType>(), alloc));
    return builder.build();
}

TEST(SanityCheck, stack_alloc) {
    const auto values = {
        Value::i32(42),
        Value::i64(42),
        Value::i8(42),
        Value::u8(42),
        Value::u32(42),
        Value::i64(42),
    };

    for (const auto& val: values) {
        const auto buffer = jit_compile_and_assembly(stack_alloc(val));
        const auto fn = buffer.code_start_as<std::int8_t()>("stackalloc").value();
        const auto res = fn();
        ASSERT_EQ(res, 42) << "Failed for value: " << val;
    }
}

TEST(SanityCheck, stack_alloc_f32) {
    constexpr auto val = Value::f32(42);
    const auto buffer = jit_compile_and_assembly(stack_alloc(val));
    const auto fn = buffer.code_start_as<float()>("stackalloc").value();
    const auto res = fn();
    ASSERT_EQ(res, 42) << "Failed for value: " << val;
}

TEST(SanityCheck, stack_alloc_f64) {
    constexpr auto val = Value::f64(42);
    const auto buffer = jit_compile_and_assembly(stack_alloc(val));
    const auto fn = buffer.code_start_as<double()>("stackalloc").value();
    const auto res = fn();
    ASSERT_EQ(res, 42) << "Failed for value: " << val;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}