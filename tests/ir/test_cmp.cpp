#include <gtest/gtest.h>

#include "utility/Error.h"

#include "helpers/Jit.h"
#include "mir/mir.h"

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
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_neg", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        fn(fn_builder.value(), IcmpPredicate::Lt,  threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_le", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        fn(fn_builder.value(), IcmpPredicate::Le, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_gt", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        fn(fn_builder.value(), IcmpPredicate::Gt, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_eq", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        fn(fn_builder.value(), IcmpPredicate::Eq, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_ne", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        fn(fn_builder.value(), IcmpPredicate::Ne, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_ge", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        fn(fn_builder.value(), IcmpPredicate::Ge, threshold);
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
        const auto is_neg = buffer0.code_start_as<std::int8_t(int)>("is_neg").value();
        const auto is_le = buffer0.code_start_as<std::int8_t(int)>("is_le").value();
        const auto is_gt = buffer0.code_start_as<std::int8_t(int)>("is_gt").value();
        const auto is_eq = buffer0.code_start_as<std::int8_t(int)>("is_eq").value();
        const auto is_ne = buffer0.code_start_as<std::int8_t(int)>("is_ne").value();
        const auto is_ge = buffer0.code_start_as<std::int8_t(int)>("is_ge").value();

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


static void is_float_predicate_impl(const FunctionBuilder& data, const FcmpPredicate pred, const Value& threshold) {
    const auto arg0 = data.arg(0);
    const auto is_neg = data.fcmp(pred, arg0, threshold);
    const auto res = data.flag2int(SignedIntegerType::i8(), is_neg);
    data.ret(res);
}

static Module is_unord_float_predicate(const FloatingPointType* ty, const Value& threshold) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_neg", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Ult,  threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_le", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Ule, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_gt", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Ugt, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_eq", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Ueq, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_ne", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Une, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_ge", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Uge, threshold);
    }

    return builder.build();
}

static const std::unordered_map<std::string, std::size_t> fp_predicate_asm_size = {
    {"is_neg", 3},
    {"is_le", 3},
    {"is_gt", 3},
    {"is_eq", 3},
    {"is_ne", 3},
    {"is_ge", 3}
};

TEST(SanityCheck, is_unord_f32_predicate) {
    const std::vector<float> values = {1., 2., 42., 100., 1000., static_cast<float>(UINT32_MAX)};

    for (const auto j: values) {
        const auto buffer0 = jit_compile_and_assembly({}, is_unord_float_predicate(FloatingPointType::f32(), Value::f32(j)), fp_predicate_asm_size, true);
        const auto is_neg = buffer0.code_start_as<std::int8_t(float)>("is_neg").value();
        const auto is_le = buffer0.code_start_as<std::int8_t(float)>("is_le").value();
        const auto is_gt = buffer0.code_start_as<std::int8_t(float)>("is_gt").value();
        const auto is_eq = buffer0.code_start_as<std::int8_t(float)>("is_eq").value();
        const auto is_ne = buffer0.code_start_as<std::int8_t(float)>("is_ne").value();
        const auto is_ge = buffer0.code_start_as<std::int8_t(float)>("is_ge").value();

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

TEST(SanityCheck, is_unord_f64_predicate) {
    const std::vector values = {1., 2., 42., 100., 1000., static_cast<double>(UINT32_MAX)};

    for (const auto j: values) {
        const auto buffer0 = jit_compile_and_assembly({}, is_unord_float_predicate(FloatingPointType::f64(), Value::f64(j)), fp_predicate_asm_size, true);
        const auto is_neg = buffer0.code_start_as<std::int8_t(double)>("is_neg").value();
        const auto is_le = buffer0.code_start_as<std::int8_t(double)>("is_le").value();
        const auto is_gt = buffer0.code_start_as<std::int8_t(double)>("is_gt").value();
        const auto is_eq = buffer0.code_start_as<std::int8_t(double)>("is_eq").value();
        const auto is_ne = buffer0.code_start_as<std::int8_t(double)>("is_ne").value();
        const auto is_ge = buffer0.code_start_as<std::int8_t(double)>("is_ge").value();

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

static Module is_ord_float_predicate(const FloatingPointType* ty, const Value& threshold) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_neg", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Olt,  threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_le", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Ole, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_gt", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Ogt, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_eq", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Oeq, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_ne", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::One, threshold);
    }
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "is_ge", FunctionBind::DEFAULT);
        auto fn_builder = builder.make_function_builder(prototype);
        is_float_predicate_impl(fn_builder.value(), FcmpPredicate::Oge, threshold);
    }

    return builder.build();
}

TEST(SanityCheck, is_ord_f32_predicate) {
    const std::vector<float> values = {1., 2., 42., 100., 1000., static_cast<float>(UINT32_MAX)};

    for (const auto j: values) {
        const auto buffer0 = jit_compile_and_assembly({}, is_ord_float_predicate(FloatingPointType::f32(), Value::f32(j)), fp_predicate_asm_size, true);
        const auto is_neg = buffer0.code_start_as<std::int8_t(float)>("is_neg").value();
        const auto is_le = buffer0.code_start_as<std::int8_t(float)>("is_le").value();
        const auto is_gt = buffer0.code_start_as<std::int8_t(float)>("is_gt").value();
        const auto is_eq = buffer0.code_start_as<std::int8_t(float)>("is_eq").value();
        const auto is_ne = buffer0.code_start_as<std::int8_t(float)>("is_ne").value();
        const auto is_ge = buffer0.code_start_as<std::int8_t(float)>("is_ge").value();

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

TEST(SanityCheck, is_ord_f64_predicate) {
    const std::vector values = {1., 2., 42., 100., 1000., static_cast<double>(UINT32_MAX)};

    for (const auto j: values) {
        const auto buffer0 = jit_compile_and_assembly({}, is_ord_float_predicate(FloatingPointType::f64(), Value::f64(j)), fp_predicate_asm_size, true);
        const auto is_neg = buffer0.code_start_as<std::int8_t(double)>("is_neg").value();
        const auto is_le = buffer0.code_start_as<std::int8_t(double)>("is_le").value();
        const auto is_gt = buffer0.code_start_as<std::int8_t(double)>("is_gt").value();
        const auto is_eq = buffer0.code_start_as<std::int8_t(double)>("is_eq").value();
        const auto is_ne = buffer0.code_start_as<std::int8_t(double)>("is_ne").value();
        const auto is_ge = buffer0.code_start_as<std::int8_t(double)>("is_ge").value();

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