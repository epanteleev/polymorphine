#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "helpers/Utils.h"
#include "mir/mir.h"
#include "lir/x64/lir.h"

static Module ret_i32(const std::int32_t value) {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "ret_one");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    const auto& data = *fn_builder.value();

    data.ret(Value::i32(value));
    return builder.build();
}

TEST(SanityCheck, ret_i32) {
    for (const auto i: {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN}) {
        const auto buffer = jit_compile_and_assembly(ret_i32(i));
        const auto fn = reinterpret_cast<int(*)()>(buffer.code_start("ret_one").value());
        const auto res = fn();
        ASSERT_EQ(res, i) << "Failed for value: " << i;
    }
}

static Module ret_i64(const std::int64_t value) {
    ModuleBuilder builder;
    FunctionPrototype prototype(SignedIntegerType::i64(), {}, "ret_one");

    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    const auto& data = *fn_builder.value();

    data.ret(Value::i64(value));
    return builder.build();
}

TEST(SanityCheck, ret_i64) {
    for (const long i: {0L, 1L, -1L, 42L, -42L, 1000000L, -1000000L, INT64_MAX, INT64_MIN}) {
        const auto buffer = jit_compile_and_assembly(ret_i64(i));
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
        const auto buffer = jit_compile_and_assembly(ret_i8_u8(static_cast<std::int8_t>(i)));
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
    const auto buffer = jit_compile_and_assembly(ret_i32_arg());
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
    const auto& data = *fn_builder.value();
    const auto arg0 = data.arg(0);
    const auto arg1 = data.arg(1);
    const auto add = data.add(arg0, arg1);
    data.ret(add);

    return builder.build();
}

TEST(SanityCheck, add_i32_args) {
    const auto buffer = jit_compile_and_assembly(add_i32_args(SignedIntegerType::i32()), true);
    const auto fn = reinterpret_cast<int(*)(int, int)>(buffer.code_start("add").value());

    std::vector values = {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN};
    for (const auto i: values) {
        for (const auto j: values) {
            const auto res = fn(i, j);
            ASSERT_EQ(res, add_overflow(i, j)) << "Failed for values: " << i << ", " << j;
        }
    }
}

TEST(SanityCheck, add_i64_args) {
    const auto buffer = jit_compile_and_assembly(add_i32_args(SignedIntegerType::i64()));
    const auto fn = reinterpret_cast<long(*)(long, long)>(buffer.code_start("add").value());

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
    FunctionPrototype prototype(SignedIntegerType::i32(), {}, "ret");
    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();

    auto* cont = data.create_basic_block();
    data.br(cont);
    data.switch_block(cont);
    data.ret(Value::i32(10));

    return builder.build();
}

TEST(SanityCheck, branch1) {
    const auto buffer = jit_compile_and_assembly(branch(), true);
    std::cout << buffer << std::endl;
    const auto fn = reinterpret_cast<int(*)()>(buffer.code_start("ret").value());
    const auto res = fn();
    ASSERT_EQ(res, 10) << "Failed for value: " << 0;
}

static void is_predicate_impl(const FunctionBuilder& data, const IcmpPredicate pred, const Value& threshold) {
    const auto arg0 = data.arg(0);
    const auto is_neg = data.icmp(pred, arg0, threshold);
    const auto res = data.flag2int(SignedIntegerType::i8(), is_neg);
    data.ret(res);
}

template<typename Fn>
static Module is_predicate_base(Fn&& fn, const IntegerType* ty, const Value& threshold) {
    ModuleBuilder builder;
    {
        FunctionPrototype prototype(ty, {ty}, "is_neg");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        fn(*fn_builder.value(), IcmpPredicate::Lt,  threshold);
    }
    {
        FunctionPrototype prototype(ty, {ty}, "is_le");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        fn(*fn_builder.value(), IcmpPredicate::Le, threshold);
    }
    {
        FunctionPrototype prototype(ty, {ty}, "is_gt");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        fn(*fn_builder.value(), IcmpPredicate::Gt, threshold);
    }
    {
        FunctionPrototype prototype(ty, {ty}, "is_eq");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        fn(*fn_builder.value(), IcmpPredicate::Eq, threshold);
    }
    {
        FunctionPrototype prototype(ty, {ty}, "is_ne");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        fn(*fn_builder.value(), IcmpPredicate::Ne, threshold);
    }
    {
        FunctionPrototype prototype(ty, {ty}, "is_ge");
        const auto fn_builder = builder.make_function_builder(std::move(prototype));
        fn(*fn_builder.value(), IcmpPredicate::Ge, threshold);
    }

    return builder.build();
}

static Module is_predicate(const IntegerType* ty, const Value& threshold) {
    return is_predicate_base(is_predicate_impl, ty, threshold);
}

TEST(SanityCheck, is_i32_predicate) {
    const auto values = {0, 1, -1, 42, -42, 1000000, -1000000, INT32_MAX, INT32_MIN};

    for (const auto j: values) {
        const auto buffer0 = jit_compile_and_assembly(is_predicate(SignedIntegerType::i32(), Value::i32(j)));
        const auto is_neg = reinterpret_cast<std::int8_t(*)(int)>(buffer0.code_start("is_neg").value());
        const auto is_le = reinterpret_cast<std::int8_t(*)(int)>(buffer0.code_start("is_le").value());
        const auto is_gt = reinterpret_cast<std::int8_t(*)(int)>(buffer0.code_start("is_gt").value());
        const auto is_eq = reinterpret_cast<std::int8_t(*)(int)>(buffer0.code_start("is_eq").value());
        const auto is_ne = reinterpret_cast<std::int8_t(*)(int)>(buffer0.code_start("is_ne").value());
        const auto is_ge = reinterpret_cast<std::int8_t(*)(int)>(buffer0.code_start("is_ge").value());

        for (const auto i: values) {
            const auto res = is_neg(i);
            ASSERT_EQ(res, i < j ? 1 : 0) << "Failed for value: " << i;

            const auto res_le = is_le(i);
            ASSERT_EQ(res_le, i <= j ? 1 : 0) << "Failed for value: " << i;

            const auto res_gt = is_gt(i);
            ASSERT_EQ(res_gt, i > j ? 1 : 0) << "Failed for value: " << i;

            const auto res_eq = is_eq(i);
            ASSERT_EQ(res_eq, i == j ? 1 : 0) << "Failed for value: " << i;

            const auto res_ne = is_ne(i);
            ASSERT_EQ(res_ne, i != j ? 1 : 0) << "Failed for value: " << i;

            const auto res_ge = is_ge(i);
            ASSERT_EQ(res_ge, i >= j ? 1 : 0) << "Failed for value: " << i;
        }
    }
}

TEST(SanityCheck, is_u32_predicate) {
    const auto values = {0U, 1U, 2U, 42U, 100U, 1000U, UINT32_MAX};

    for (const auto j: values) {
        const auto buffer0 = jit_compile_and_assembly(is_predicate(UnsignedIntegerType::u32(), Value::u32(j)), true);
        const auto is_neg = buffer0.code_start_as<std::int8_t(unsigned int)>("is_neg").value();
        const auto is_le = buffer0.code_start_as<std::int8_t(unsigned int)>("is_le").value();
        const auto is_gt = buffer0.code_start_as<std::int8_t(unsigned int)>("is_gt").value();
        const auto is_eq = buffer0.code_start_as<std::int8_t(unsigned int)>("is_eq").value();
        const auto is_ne = buffer0.code_start_as<std::int8_t(unsigned int)>("is_ne").value();
        const auto is_ge = buffer0.code_start_as<std::int8_t(unsigned int)>("is_ge").value();

        for (const auto i: values) {
            const auto res = is_neg(i);
            ASSERT_EQ(res, i < j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_le = is_le(i);
            ASSERT_EQ(res_le, i <= j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_gt = is_gt(i);
            ASSERT_EQ(res_gt, i > j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_eq = is_eq(i);
            ASSERT_EQ(res_eq, i == j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_ne = is_ne(i);
            ASSERT_EQ(res_ne, i != j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_ge = is_ge(i);
            ASSERT_EQ(res_ge, i >= j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;
        }
    }
}

static Module stack_alloc(const Value& val) {
    ModuleBuilder builder;
    FunctionPrototype prototype(val.type(), {}, "stackalloc");
    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    const auto& data = *fn_builder.value();
    const auto alloc = data.alloc(dynamic_cast<const PrimitiveType*>(val.type()));
    data.store(alloc, val);
    data.ret(data.load(dynamic_cast<const PrimitiveType*>(val.type()), alloc));
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

static void is_predicate_branch_impl(FunctionBuilder& data, const IcmpPredicate pred, const Value& threshold) {
    const auto then = data.create_basic_block();
    const auto else_ = data.create_basic_block();
    const auto cont = data.create_basic_block();

    const auto arg0 = data.arg(0);
    const auto res = data.alloc(SignedIntegerType::i32());
    const auto icmp = data.icmp(pred, arg0, threshold);
    data.br_cond(icmp, then, else_);

    data.switch_block(then);
    data.store(res, Value::i32(1));
    data.br(cont);

    data.switch_block(else_);
    data.store(res, Value::i32(0));
    data.br(cont);

    data.switch_block(cont);
    data.ret(data.load(SignedIntegerType::i32(), res));
}

static Module is_predicate_branch(const IntegerType* ty, const Value& threshold) {
    return is_predicate_base(is_predicate_branch_impl, ty, threshold);
}

TEST(SanityCheck, branch_u32_predicate) {
    const auto values = {0U, 1U, 2U, 42U, 100U, 1000U, UINT32_MAX};

    for (const auto j: values) {
        const auto buffer0 = jit_compile_and_assembly(is_predicate_branch(UnsignedIntegerType::u32(), Value::u32(j)));
        const auto is_neg = buffer0.code_start_as<std::int8_t(unsigned int)>("is_neg").value();
        const auto is_le = buffer0.code_start_as<std::int8_t(unsigned int)>("is_le").value();
        const auto is_gt = buffer0.code_start_as<std::int8_t(unsigned int)>("is_gt").value();
        const auto is_eq = buffer0.code_start_as<std::int8_t(unsigned int)>("is_eq").value();
        const auto is_ne = buffer0.code_start_as<std::int8_t(unsigned int)>("is_ne").value();
        const auto is_ge = buffer0.code_start_as<std::int8_t(unsigned int)>("is_ge").value();

        for (const auto i: values) {
            const auto res = is_neg(i);
            ASSERT_EQ(res, i < j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_le = is_le(i);
            ASSERT_EQ(res_le, i <= j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_gt = is_gt(i);
            ASSERT_EQ(res_gt, i > j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_eq = is_eq(i);
            ASSERT_EQ(res_eq, i == j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_ne = is_ne(i);
            ASSERT_EQ(res_ne, i != j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;

            const auto res_ge = is_ge(i);
            ASSERT_EQ(res_ge, i >= j ? 1 : 0) << "Failed for value: " << i << " with threshold: " << j;
        }
    }
}


static Module swap(const PrimitiveType* ty) {
    ModuleBuilder builder;
    FunctionPrototype prototype(VoidType::type(), {PointerType::ptr(), PointerType::ptr()}, "swap");
    const auto fn_builder = builder.make_function_builder(std::move(prototype));
    auto& data = *fn_builder.value();
    const auto arg0 = data.arg(0);
    const auto arg1 = data.arg(1);
    const auto tmp0 = data.load(ty, arg0);
    const auto tmp1 = data.load(ty, arg1);

    data.store(arg0, tmp1);
    data.store(arg1, tmp0);
    data.ret();

    return builder.build();
}

TEST(SanityCheck, swap_signed) {
    const auto sign_types = {
        SignedIntegerType::i32(),
        SignedIntegerType::i64(),
        SignedIntegerType::i16(),
        SignedIntegerType::i8(),
    };

    for (const auto& ty: sign_types) {
        const auto buffer = jit_compile_and_assembly(swap(ty));
        const auto fn = buffer.code_start_as<void(void*, void*)>("swap").value();

        std::int64_t a = 42;
        std::int64_t b = 84;
        fn(&a, &b);

        ASSERT_EQ(a, 84) << "Failed to swap a for type: " << *ty;
        ASSERT_EQ(b, 42) << "Failed to swap b for type: ";
    }
}

TEST(SanityCheck, swap_unsigned) {
    const auto sign_types = {
        UnsignedIntegerType::u32(),
        UnsignedIntegerType::u64(),
        UnsignedIntegerType::u16(),
        UnsignedIntegerType::u8(),
    };

    for (const auto& ty: sign_types) {
        const auto buffer = jit_compile_and_assembly(swap(ty));
        const auto fn = buffer.code_start_as<void(void*, void*)>("swap").value();

        std::uint64_t a = 42;
        std::uint64_t b = 84;
        fn(&a, &b);

        ASSERT_EQ(a, 84) << "Failed to swap a for type: " << *ty;
        ASSERT_EQ(b, 42) << "Failed to swap b for type: ";
    }
}

TEST(SanityCheck, swap_ptr) {
    const auto ty = PointerType::ptr();
    const auto buffer = jit_compile_and_assembly(swap(ty));
    const auto fn = buffer.code_start_as<void(void*, void*)>("swap").value();

    int a = 42;
    int b = 84;
    int* pa = &a;
    int* pb = &b;
    fn(&pa, &pb);

    ASSERT_EQ(*pa, 84) << "Failed to swap a for type: " << *ty;
    ASSERT_EQ(*pb, 42) << "Failed to swap b for type: ";
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}