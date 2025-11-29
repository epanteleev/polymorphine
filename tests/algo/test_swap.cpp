#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"
#include "utility/Error.h"

static Module swap(const PrimitiveType* ty) {
    ModuleBuilder builder;
    const auto prototype = builder.add_function_prototype(VoidType::type(), {PointerType::ptr(), PointerType::ptr()}, "swap", FunctionBind::DEFAULT);
    auto fn_builder = builder.make_function_builder(prototype);
    const auto data = fn_builder.value();
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

TEST(SanityCheck, swap_f32) {
    const auto buffer = jit_compile_and_assembly(swap(FloatingPointType::f32()));
    const auto fn = buffer.code_start_as<void(void*, void*)>("swap").value();

    float a = 42;
    float b = 84;
    fn(&a, &b);

    ASSERT_EQ(a, 84);
    ASSERT_EQ(b, 42) << "Failed to swap b for type: ";
}

TEST(SanityCheck, swap_f64) {
    const auto buffer = jit_compile_and_assembly(swap(FloatingPointType::f64()));
    const auto fn = buffer.code_start_as<void(void*, void*)>("swap").value();

    double a = 42;
    double b = 84;
    fn(&a, &b);

    ASSERT_EQ(a, 84);
    ASSERT_EQ(b, 42) << "Failed to swap b for type: ";
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