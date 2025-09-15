#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module global_constant_test(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {}, "global_constant_test", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", ty, 42L).value();
        const auto value = data.load(ty, constant);
        data.ret(value);
    }
    return builder.build();
}

TEST(GlobalConstant, load_global_constant_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_constant_test", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, global_constant_test(SignedIntegerType::i64()), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("global_constant_test").value();
    ASSERT_EQ(fn(), 42);
}

TEST(GlobalConstant, load_global_constant_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
            {"global_constant_test", 2},
        };

    const auto buffer = jit_compile_and_assembly({}, global_constant_test(SignedIntegerType::i32()), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("global_constant_test").value();
    ASSERT_EQ(fn(), 42);
}

static Module global_constant_test_string() {
    ModuleBuilder builder;
    {
        const auto array_type = builder.add_array_type(SignedIntegerType::i8(), 8);
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "global_constant_test", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", array_type, "success").value();
        data.ret(constant);
    }
    return builder.build();
}

TEST(GlobalConstant, get_string) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_constant_test", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, global_constant_test_string(), asm_size, true);
    const auto fn = buffer.code_start_as<char*()>("global_constant_test").value();
    ASSERT_STREQ(fn(), "success");
}

#ifndef NDEBUG
TEST(GlobalConstant, wrond_constant) {
    ModuleBuilder builder;
    // wrong type
    const auto constant = builder.add_constant("my_global_const", builder.add_array_type(SignedIntegerType::i64(), 8), "my_global_const");
    ASSERT_TRUE(!constant.has_value());
    ASSERT_EQ(constant.error(), Error::InvalidArgument);

    const auto constant2 = builder.add_constant("my_global_const", builder.add_array_type(SignedIntegerType::i8(), 2), "my_global_const");
    ASSERT_TRUE(!constant2.has_value());
    ASSERT_EQ(constant2.error(), Error::InvalidArgument);

    const auto module = builder.build();
    ASSERT_TRUE(module.constant_pool().empty());
}
#endif

static Module write_constant_in_argument(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(VoidType::type(), {PointerType::ptr()}, "write_constant_in_argument", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto constant = builder.add_constant("my_global_const", ty, 144).value();
        const auto value = data.load(ty, constant);
        data.store(arg0, value);
        data.ret();
    }
    return builder.build();
}

TEST(GlobalConstant, write_constant_in_argument) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"write_constant_in_argument", 3},
    };

    const auto buffer = jit_compile_and_assembly({}, write_constant_in_argument(SignedIntegerType::i8()), asm_size, true);
    const auto fn = buffer.code_start_as<void(std::int8_t*)>("write_constant_in_argument").value();
    std::int8_t value{};
    fn(&value);
    ASSERT_EQ(value, static_cast<std::int8_t>(144));
}

static std::int16_t deref_int16(const std::int16_t* p) {
    return *p;
}

static Module escape_constant(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {}, "escape_constant", FunctionBind::DEFAULT);
        const auto deref_prototype = builder.add_function_prototype(ty, {PointerType::ptr()}, "deref_int16", FunctionBind::EXTERN);

        auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", ty, 256).value();
        const auto cont = data.create_basic_block();
        const auto call = data.call(deref_prototype, cont, {constant});
        data.switch_block(cont);
        data.ret(call);
    }
    return builder.build();
}

TEST(GlobalConstant, escape_constant) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"escape_constant", 3},
    };

    const auto external_symbols = std::unordered_map<std::string, std::size_t>{
        {"deref_int16", reinterpret_cast<std::size_t>(&deref_int16)},
    };

    const auto buffer = jit_compile_and_assembly(external_symbols, escape_constant(SignedIntegerType::i16()), asm_size, true);
    const auto fn = buffer.code_start_as<std::int16_t()>("escape_constant").value();
    ASSERT_EQ(fn(), 256);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}