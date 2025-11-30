#include <gtest/gtest.h>

#include "../../helpers/Jit.h"
#include "mir/mir.h"

static Module global_constant_test(const PrimitiveType* ty) {
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
    GTEST_SKIP();
    ModuleBuilder builder;
    // wrong type
    const auto constant = builder.add_constant("my_global_const", builder.add_array_type(SignedIntegerType::i64(), 8), "my_global_const");
    ASSERT_TRUE(!constant.has_value());
    ASSERT_EQ(constant.error(), Error::InvalidArgument);

    const auto constant2 = builder.add_constant("my_global_const", builder.add_array_type(SignedIntegerType::i8(), 2), "my_global_const");
    ASSERT_TRUE(!constant2.has_value());
    ASSERT_EQ(constant2.error(), Error::InvalidArgument);

    const auto module = builder.build();
    ASSERT_TRUE(module.gvalue_pool().empty());
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

static Module global_constant_struct() {
    ModuleBuilder builder;
    {
        const auto struct_type = builder.add_struct_type("MyStruct", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        const auto prototype = builder.add_function_prototype(struct_type, {}, "global_constant_test", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", struct_type, Initializer{42L, 84L}).value();
        data.ret(constant);
    }
    return builder.build();
}

TEST(GlobalConstant, load_global_constant_struct) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_constant_test", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, global_constant_struct(), asm_size, true);
    struct MyStruct {
        std::int64_t a;
        std::int64_t b;
    };
    const auto fn = buffer.code_start_as<MyStruct*()>("global_constant_test").value();
    const auto result = fn();
    ASSERT_EQ(result->a, 42);
    ASSERT_EQ(result->b, 84);
}

static Module inner_constant_struct() {
    ModuleBuilder builder;
    {
        const auto inner_struct_type = builder.add_struct_type("InnerStruct", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        const auto struct_type = builder.add_struct_type("MyStruct", {inner_struct_type, SignedIntegerType::i64()});
        const auto prototype = builder.add_function_prototype(struct_type, {}, "global_constant_test", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", struct_type, Initializer{Initializer{42L, 84L}, 168L}).value();
        data.ret(constant);
    }
    return builder.build();
}

struct InnerStruct {
    std::int64_t a;
    std::int64_t b;
};

struct MyStruct {
    InnerStruct inner;
    std::int64_t c;
};

TEST(GlobalConstant, load_global_constant_inner_struct) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"global_constant_test", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, inner_constant_struct(), asm_size, true);
    const auto fn = buffer.code_start_as<MyStruct*()>("global_constant_test").value();
    const auto result = fn();
    ASSERT_EQ(result->inner.a, 42);
    ASSERT_EQ(result->inner.b, 84);
    ASSERT_EQ(result->c, 168);
}

static Module get_pointer_to_struct_field() {
    ModuleBuilder builder;
    {
        const auto struct_type = builder.add_struct_type("MyStruct", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        const auto constant = builder.add_constant("my_global_const", struct_type, Initializer{1L, 2L}).value();
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "get_pointer_to_struct_field", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto field_ptr = data.gfp(struct_type, constant, 1);
        data.ret(field_ptr);
    }
    return builder.build();
}

TEST(GlobalConstant, get_pointer_to_struct_field) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"get_pointer_to_struct_field", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, get_pointer_to_struct_field(), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t*()>("get_pointer_to_struct_field").value();
    const auto result = fn();
    ASSERT_EQ(*result, 2);
}

static Module get_pointer_to_inner_struct_field() {
    ModuleBuilder builder;
    {
        const auto inner_struct_type = builder.add_struct_type("StringHolder", {PointerType::ptr()});
        const auto string_literal = builder.add_constant("my_string_literal", builder.add_array_type(SignedIntegerType::i8(), 6), "hello").value();

        const auto constant = builder.add_constant("my_global_const", inner_struct_type, Initializer{string_literal}).value();
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "get_pointer_to_inner_struct_field", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto field_ptr = data.gfp(inner_struct_type, constant, 0);
        const auto str_ptr = data.load(PointerType::ptr(), field_ptr);
        data.ret(str_ptr);
    }
    return builder.build();
}

TEST(GlobalConstant, get_pointer_to_inner_struct_field) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"get_pointer_to_inner_struct_field", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, get_pointer_to_inner_struct_field(), asm_size, true);
    const auto fn = buffer.code_start_as<const char*()>("get_pointer_to_inner_struct_field").value();
    const auto result = fn();
    ASSERT_STREQ(result, "hello");
}

static Module escape_constant_in_argument() {
    ModuleBuilder builder;
    {
        const auto sum_fintype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "sum", FunctionBind::EXTERN);
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "escape_constant_in_argument", FunctionBind::DEFAULT);
        const auto struct_type = builder.add_struct_type("MyStruct", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", struct_type, Initializer{42L, 84L}).value();
        const auto cont = data.create_basic_block();
        const auto call = data.call(sum_fintype, cont, {constant});
        data.switch_block(cont);
        data.ret(call);
    }
    return builder.build();
}

struct Point2 {
    std::int64_t x;
    std::int64_t y;
};

static std::int64_t sum(const Point2* p) {
    return p->x + p->y;
}

TEST(GlobalConstant, escape_struct_constant_in_argument) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"escape_constant_in_argument", 3},
    };
    const auto external_symbols = std::unordered_map<std::string, std::size_t>{
        {"sum", reinterpret_cast<std::size_t>(&sum)},
    };
    const auto buffer = jit_compile_and_assembly(external_symbols, escape_constant_in_argument(), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("escape_constant_in_argument").value();
    const auto res = fn();
    ASSERT_EQ(res, 126);
}

static Module escape_field_constant_in_argument() {
    ModuleBuilder builder;
    {
        const auto sum_fintype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "deref", FunctionBind::EXTERN);
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "escape_field_constant_in_argument", FunctionBind::DEFAULT);
        const auto struct_type = builder.add_struct_type("MyStruct", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        auto data = builder.make_function_builder(prototype).value();
        const auto constant = builder.add_constant("my_global_const", struct_type, Initializer{42L, 84L}).value();

        const auto field2 = data.gfp(struct_type, constant, 1);
        const auto cont = data.create_basic_block();
        const auto call = data.call(sum_fintype, cont, {field2});
        data.switch_block(cont);
        data.ret(call);
    }
    return builder.build();
}

static std::int64_t deref_i64(const std::int64_t* p) {
    return *p;
}

TEST(GlobalConstant, escape_struct_field_constant_in_argument) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"escape_field_constant_in_argument", 3},
    };
    const auto external_symbols = std::unordered_map<std::string, std::size_t>{
        {"deref", reinterpret_cast<std::size_t>(&deref_i64)}
    };

    const auto buffer = jit_compile_and_assembly(external_symbols, escape_field_constant_in_argument(), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("escape_field_constant_in_argument").value();
    const auto res = fn();
    ASSERT_EQ(res, 84);
}

static Module escape_array_slice_constant_in_argument(const IntegerType* ty) {
    ModuleBuilder builder;
    {
        const auto sum_fintype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr(), SignedIntegerType::i64()}, "sum", FunctionBind::EXTERN);
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "escape_array_slice_constant_in_argument", FunctionBind::DEFAULT);
        const auto array_type = builder.add_array_type(ty, 5);
        auto data = builder.make_function_builder(prototype).value();
        const auto constant = data.add_constant("my_global_const", array_type, Initializer{42L, 84L, 75L, 89, 12}).value();
        const auto third = data.gep(ty, constant, Value::i64(2));
        const auto cont = data.create_basic_block();
        const auto call = data.call(sum_fintype, cont, {third, Value::i64(3)});
        data.switch_block(cont);
        data.ret(call);
    }

    return builder.build();
}

template<std::integral T>
static std::int64_t sum_slice(const T* p, const std::size_t size) {
    std::int64_t sum{};
    for (std::size_t i{}; i < size; ++i) {
        sum += p[i];
    }
    return sum;
}

TEST(GlobalConstant, espace_array_slice_constant_in_argument_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"escape_array_slice_constant_in_argument", 4},
    };
    const std::unordered_map<std::string, std::size_t> external_symbols {
        {"sum", reinterpret_cast<std::size_t>(&sum_slice<std::int64_t>)}
    };
    const auto buffer = jit_compile_and_assembly(external_symbols, escape_array_slice_constant_in_argument(SignedIntegerType::i64()), asm_size);
    const auto fn = buffer.code_start_as<std::int64_t()>("escape_array_slice_constant_in_argument").value();
    const auto res = fn();
    ASSERT_EQ(res, 75L + 89 + 12);
}

TEST(GlobalConstant, espace_array_slice_constant_in_argument_i32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"escape_array_slice_constant_in_argument", 4},
    };
    const std::unordered_map<std::string, std::size_t> external_symbols {
        {"sum", reinterpret_cast<std::size_t>(&sum_slice<std::int32_t>)}
    };
    const auto buffer = jit_compile_and_assembly(external_symbols, escape_array_slice_constant_in_argument(SignedIntegerType::i32()), asm_size);
    const auto fn = buffer.code_start_as<std::int64_t()>("escape_array_slice_constant_in_argument").value();
    const auto res = fn();
    ASSERT_EQ(res, 75L + 89 + 12);
}

TEST(GlobalConstant, espace_array_slice_constant_in_argument_i16) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"escape_array_slice_constant_in_argument", 4},
    };
    const std::unordered_map<std::string, std::size_t> external_symbols {
        {"sum", reinterpret_cast<std::size_t>(&sum_slice<std::int16_t>)}
    };
    const auto buffer = jit_compile_and_assembly(external_symbols, escape_array_slice_constant_in_argument(SignedIntegerType::i16()), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("escape_array_slice_constant_in_argument").value();
    const auto res = fn();
    ASSERT_EQ(res, 75L + 89 + 12);
}

TEST(GlobalConstant, espace_array_slice_constant_in_argument_i8) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"escape_array_slice_constant_in_argument", 4},
    };
    const std::unordered_map<std::string, std::size_t> external_symbols {
        {"sum", reinterpret_cast<std::size_t>(&sum_slice<std::int8_t>)}
    };
    const auto buffer = jit_compile_and_assembly(external_symbols, escape_array_slice_constant_in_argument(SignedIntegerType::i8()), asm_size, true);
    const auto fn = buffer.code_start_as<std::int64_t()>("escape_array_slice_constant_in_argument").value();
    const auto res = fn();
    ASSERT_EQ(res, 75L + 89 + 12);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}