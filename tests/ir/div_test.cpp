#include <cmath>
#include <gtest/gtest.h>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module div(const FloatingPointType* ty) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty, ty}, "idiv", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto b = data.arg(1);
        const auto c = data.div(a, b);
        data.ret(c);
    }
    return builder.build();
}

TEST(Div, basic_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, div(FloatingPointType::f64()), asm_size);
    const auto idiv = buffer.code_start_as<double(double, double)>("idiv").value();
    const auto p = idiv(4, 2);
    ASSERT_DOUBLE_EQ(p, 2);
}

TEST(Div, basic_f32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"idiv", 2},
    };

    const auto buffer = jit_compile_and_assembly({}, div(FloatingPointType::f32()), asm_size);
    const auto idiv = buffer.code_start_as<float(float, float)>("idiv").value();
    const auto p = idiv(4, 2);
    ASSERT_FLOAT_EQ(p, 2);
}

static Module div_cst(const FloatingPointType* ty, const Value& val) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "div_cst", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto c = data.div(a, val);
        data.ret(c);
    }
    return builder.build();
}

TEST(Div, cst_f32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"div_cst", 2},
    };

    const auto mod = div_cst(FloatingPointType::f32(), Value::f32(2));
    const auto buffer = jit_compile_and_assembly({}, mod, asm_size);
    const auto idiv = buffer.code_start_as<float(float)>("div_cst").value();
    const auto p = idiv(4);
    ASSERT_FLOAT_EQ(p, 2);
}

TEST(Div, cst_f64_zero) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"div_cst", 3},
    };

    const auto mod = div_cst(FloatingPointType::f64(), Value::f64(0));
    const auto buffer = jit_compile_and_assembly({}, mod, asm_size);
    const auto idiv = buffer.code_start_as<double(double)>("div_cst").value();
    const auto p = idiv(0);
    ASSERT_TRUE(std::isnan(p));
}

static Module div_cst0(const FloatingPointType* ty, const Value& val) {
    ModuleBuilder builder;
    {
        const auto prototype = builder.add_function_prototype(ty, {ty}, "div_cst", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        const auto a = data.arg(0);
        const auto c = data.div(val, a);
        data.ret(c);
    }
    return builder.build();
}

TEST(Div, cst0_f32) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"div_cst", 4},
    };

    const auto mod = div_cst0(FloatingPointType::f32(), Value::f32(2));
    const auto buffer = jit_compile_and_assembly({}, mod, asm_size, true);
    const auto idiv = buffer.code_start_as<float(float)>("div_cst").value();
    const auto p = idiv(4);
    ASSERT_FLOAT_EQ(p, 0.5);
}

TEST(Div, cst0_f64_zero) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"div_cst", 4},
    };

    const auto mod = div_cst0(FloatingPointType::f64(), Value::f64(0));
    const auto buffer = jit_compile_and_assembly({}, mod, asm_size, true);
    const auto idiv = buffer.code_start_as<double(double)>("div_cst").value();
    const auto p = idiv(4);
    ASSERT_DOUBLE_EQ(p, 0);
}

int main(int argc, char **argv) {
    error::setup_terminate_handler();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}