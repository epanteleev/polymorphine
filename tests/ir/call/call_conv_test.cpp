#include <gtest/gtest.h>
#include <climits>

#include "../../helpers/Jit.h"
#include "mir/mir.h"

static Module struct_arg_size1(const PrimitiveType* ty) {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {ty, ty, ty});
        const auto prototype = builder.add_function_prototype(ty, {vect_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(ty, field0);
        const auto val1 = data.load(ty, field1);
        const auto val2 = data.load(ty, field2);
        const auto sum = data.add(data.add(val0, val1), val2);
        data.ret(sum);
    }

    return builder.build();
}
struct Vec {
    int64_t x;
    int64_t y;
    int64_t z;
};

struct VecF64 {
    double x;
    double y;
    double z;
};

TEST(StructArg, pass_one_by_value_double) {
    const std::unordered_map<std::string, std::size_t> asm_size {
            {"sum_fields", 6},
        };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size1(FloatingPointType::f64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<double(VecF64)>("sum_fields").value();
    VecF64 v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(v);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100);
}

TEST(StructArg, pass_one_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 6},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size1(SignedIntegerType::i64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec)>("sum_fields").value();
    Vec v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(v);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100);
}

static Module struct_arg_size2(const PrimitiveType* ty) {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {ty, ty, ty});
        const auto prototype = builder.add_function_prototype(ty, {vect_type, vect_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(ty, field0);
        const auto val1 = data.load(ty, field1);
        const auto val2 = data.load(ty, field2);
        const auto sum = data.add(data.add(val0, val1), val2);
        const auto arg1 = data.arg(1);
        const auto field3 = data.gfp(vect_type, arg1, 0);
        const auto field4 = data.gfp(vect_type, arg1, 1);
        const auto field5 = data.gfp(vect_type, arg1, 2);
        const auto val3 = data.load(ty, field3);
        const auto val4 = data.load(ty, field4);
        const auto val5 = data.load(ty, field5);
        const auto sum2 = data.add(data.add(val3, val4), val5);
        const auto total = data.add(sum, sum2);
        data.ret(total);
    }

    return builder.build();
}

TEST(StructArg, pass_two_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 12},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size2(SignedIntegerType::i64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec, Vec)>("sum_fields").value();
    Vec v1{};
    v1.x = -42;
    v1.y = static_cast<std::int64_t>(INT_MAX)+2;
    v1.z = 100;
    Vec v2{};
    v2.x = 1;
    v2.y = 2;
    v2.z = 3;
    const auto sum = sum_fields(v1, v2);
    ASSERT_EQ(sum, (static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100) + (1 + 2 + 3));
}

TEST(StructArg, pass_two_by_value_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 12},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size2(FloatingPointType::f64()), asm_size);
    const auto sum_fields = buffer.code_start_as<double(VecF64, VecF64)>("sum_fields").value();
    VecF64 v1{};
    v1.x = -42;
    v1.y = static_cast<std::int64_t>(INT_MAX)+2;
    v1.z = 100;

    VecF64 v2{};
    v2.x = 1;
    v2.y = 2;
    v2.z = 3;
    const auto sum = sum_fields(v1, v2);
    ASSERT_EQ(sum, (static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100) + (1 + 2 + 3));
}

static Module struct_arg_size3(const PrimitiveType* ty) {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {ty, ty, ty});
        auto point_type = builder.add_struct_type("Point", {ty, ty});
        const auto prototype = builder.add_function_prototype(ty, {vect_type, vect_type, point_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(ty, field0);
        const auto val1 = data.load(ty, field1);
        const auto val2 = data.load(ty, field2);
        const auto sum = data.add(data.add(val0, val1), val2);

        const auto arg1 = data.arg(1);
        const auto field3 = data.gfp(vect_type, arg1, 0);
        const auto field4 = data.gfp(vect_type, arg1, 1);
        const auto field5 = data.gfp(vect_type, arg1, 2);
        const auto val3 = data.load(ty, field3);
        const auto val4 = data.load(ty, field4);
        const auto val5 = data.load(ty, field5);
        const auto sum2 = data.add(data.add(val3, val4), val5);

        const auto arg2 = data.arg(2);
        const auto field6 = data.gfp(point_type, arg2, 0);
        const auto field7 = data.gfp(point_type, arg2, 1);
        const auto val6 = data.load(ty, field6);
        const auto val7 = data.load(ty, field7);
        const auto sum3 = data.add(val6, val7);
        const auto total = data.add(data.add(sum, sum2), sum3);
        data.ret(total);
    }
    return builder.build();
}

struct Point {
    std::int64_t x;
    std::int64_t y;
    std::int64_t z;
    std::int64_t w;
};

struct PointF64 {
    double x;
    double y;
    double z;
    double w;
};

TEST(StructArg, pass_three_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 16},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size3(SignedIntegerType::i64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec, Vec, Point)>("sum_fields").value();
    Vec v1{};
    v1.x = -42;
    v1.y = static_cast<std::int64_t>(INT_MAX)+2;
    v1.z = 100;
    Vec v2{};
    v2.x = 1;
    v2.y = 2;
    v2.z = 3;
    Point v3{};
    v3.x = 10;
    v3.y = 20;
    const auto sum = sum_fields(v1, v2, v3);
    ASSERT_EQ(sum, (static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100) + (1 + 2 + 3) + (10 + 20));
}

TEST(StructArg, pass_three_by_value_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"sum_fields", 16},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_arg_size3(FloatingPointType::f64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<double(VecF64, VecF64, PointF64)>("sum_fields").value();
    VecF64 v1{};
    v1.x = -42;
    v1.y = static_cast<double>(INT_MAX)+2;
    v1.z = 100;
    VecF64 v2{};
    v2.x = 1;
    v2.y = 2;
    v2.z = 3;
    PointF64 v3{};
    v3.x = 10;
    v3.y = 20;
    const auto sum = sum_fields(v1, v2, v3);
    ASSERT_EQ(sum, (static_cast<double>(-42) + static_cast<double>(INT_MAX)+2 + 100) + (1 + 2 + 3) + (10 + 20));
}

static Module array_of_structs_with_cst(const PrimitiveType* ty) {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {ty, ty, ty});
        const auto prototype = builder.add_function_prototype(ty, {vect_type, ty}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(ty, field0);
        const auto val1 = data.load(ty, field1);
        const auto val2 = data.load(ty, field2);
        const auto arg1 = data.arg(1);
        const auto sum = data.add(data.add(val0, val1), data.add(arg1, val2));
        data.ret(sum);
    }

    return builder.build();
}

TEST(StructArg, array_of_structs_with_cst) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 7},
    };

    const auto buffer = jit_compile_and_assembly({}, array_of_structs_with_cst(SignedIntegerType::i64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(Vec, std::int64_t)>("sum_fields").value();
    Vec v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(v, 5);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100 + 5);
}

TEST(StructArg, array_of_structs_with_cst_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 7},
    };

    const auto buffer = jit_compile_and_assembly({}, array_of_structs_with_cst(FloatingPointType::f64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<double(VecF64, double)>("sum_fields").value();
    VecF64 v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(v, 5);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100 + 5);
}

static Module array_of_structs_with_cst0(const PrimitiveType* ty) {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {ty, ty, ty});
        const auto prototype = builder.add_function_prototype(ty, {ty, vect_type}, "sum_fields", std::vector{AttributeSet{}, AttributeSet{Attribute::ByValue}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(1);
        const auto field0 = data.gfp(vect_type, arg0, 0);
        const auto field1 = data.gfp(vect_type, arg0, 1);
        const auto field2 = data.gfp(vect_type, arg0, 2);
        const auto val0 = data.load(ty, field0);
        const auto val1 = data.load(ty, field1);
        const auto val2 = data.load(ty, field2);
        const auto arg1 = data.arg(0);
        const auto sum = data.add(data.add(val0, val1), data.add(arg1, val2));
        data.ret(sum);
    }

    return builder.build();
}

TEST(StructArg, array_of_structs_with_cst0) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 7},
    };

    const auto buffer = jit_compile_and_assembly({}, array_of_structs_with_cst0(SignedIntegerType::i64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t(std::int64_t, Vec)>("sum_fields").value();
    Vec v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(5, v);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100 + 5);
}

TEST(StructArg, array_of_structs_with_cst0_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 7},
    };

    const auto buffer = jit_compile_and_assembly({}, array_of_structs_with_cst0(FloatingPointType::f64()), asm_size, true);
    const auto sum_fields = buffer.code_start_as<double(double, VecF64)>("sum_fields").value();
    VecF64 v{};
    v.x = -42;
    v.y = static_cast<std::int64_t>(INT_MAX)+2;
    v.z = 100;
    const auto sum = sum_fields(5, v);
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100 + 5);
}

struct Point2 {
    std::int64_t x;
    std::int64_t y;
};

static std::int64_t sum_ints(Point2 p) {
    return p.x + p.y;
}

struct Point2F64 {
    double x;
    double y;
};

static double sum_floats(Point2F64 p) {
    return p.x + p.y;
}

template<typename Fn>
static Module struct_arg_external(const PrimitiveType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        auto point_type = builder.add_struct_type("Point2", {ty, ty});
        const auto prototype = builder.add_function_prototype(ty, {point_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto ext_proto = builder.add_function_prototype(ty, {ty, ty}, "sum", FunctionBind::EXTERN);
        const auto cont = data.create_basic_block();
        const auto call = data.call(ext_proto, cont, {fn(20), fn(30)});
        data.switch_block(cont);
        data.ret(call);
    }

    return builder.build();
}

TEST(StructArg, pass_external_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 4},
    };

    const std::unordered_map<std::string, std::size_t> externs{
        {"sum", reinterpret_cast<std::size_t>(&sum_ints)}
    };

    const auto buffer = jit_compile_and_assembly(externs, struct_arg_external(SignedIntegerType::i64(), Value::i64), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t()>("sum_fields").value();
    const auto sum = sum_fields();
    ASSERT_EQ(sum, 50);
}

TEST(StructArg, pass_external_by_value_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 4},
    };

    const std::unordered_map<std::string, std::size_t> externs{
        {"sum", reinterpret_cast<std::size_t>(&sum_floats)}
    };

    const auto buffer = jit_compile_and_assembly(externs, struct_arg_external(FloatingPointType::f64(), Value::f64), asm_size, true);
    const auto sum_fields = buffer.code_start_as<double()>("sum_fields").value();
    const auto sum = sum_fields();
    ASSERT_EQ(sum, 50);
}

static std::int64_t sum_ints2(Vec v) {
    return v.x + v.y + v.z;
}

static Module struct_arg_external2() {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {SignedIntegerType::i64(), SignedIntegerType::i64(), SignedIntegerType::i64()});
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {vect_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto ext_proto = builder.add_function_prototype(SignedIntegerType::i64(), {vect_type}, "sum_ints2", std::vector{AttributeSet{Attribute::ByValue}}, FunctionBind::EXTERN);
        const auto alloc = data.alloc(vect_type);
        const auto field0 = data.gfp(vect_type, alloc, 0);
        const auto field1 = data.gfp(vect_type, alloc, 1);
        const auto field2 = data.gfp(vect_type, alloc, 2);
        data.store(field0, Value::i64(20));
        data.store(field1, Value::i64(30));
        data.store(field2, Value::i64(40));

        const auto cont = data.create_basic_block();
        const auto call = data.call(ext_proto, cont, {alloc});
        data.switch_block(cont);
        data.ret(call);
    }

    return builder.build();
}

TEST(StructArg, pass_external_struct_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 17},
    };

    const std::unordered_map<std::string, std::size_t> external_symbols{
        {"sum_ints2", reinterpret_cast<std::size_t>(&sum_ints2)}
    };

    const auto buffer = jit_compile_and_assembly(external_symbols, struct_arg_external2(), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t()>("sum_fields").value();
    const auto sum = sum_fields();
    ASSERT_EQ(sum, 90);
}

static std::int64_t sum_ints3(Vec v1, Vec v2) {
    return v1.x + v1.y + v1.z + v2.x + v2.y + v2.z;
}

static double sum_floats3(VecF64 v1, VecF64 v2) {
    return v1.x + v1.y + v1.z + v2.x + v2.y + v2.z;
}

template<typename Fn>
static Module struct_arg_external3(const PrimitiveType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        auto vect_type = builder.add_struct_type("Vec", {ty, ty, ty});
        const auto prototype = builder.add_function_prototype(ty, {vect_type, vect_type}, "sum_fields", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}}, FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto ext_proto = builder.add_function_prototype(ty, {vect_type, vect_type}, "sum", std::vector{AttributeSet{Attribute::ByValue}, AttributeSet{Attribute::ByValue}}, FunctionBind::EXTERN);

        const auto alloc0 = data.alloc(SignedIntegerType::i32());
        data.store(alloc0, Value::i32(1));
        const auto alloc1 = data.alloc(vect_type);
        const auto field0 = data.gfp(vect_type, alloc1, 0);
        const auto field1 = data.gfp(vect_type, alloc1, 1);
        const auto field2 = data.gfp(vect_type, alloc1, 2);
        data.store(field0, fn(20));
        data.store(field1, fn(30));
        data.store(field2, fn(40));

        const auto alloc2 = data.alloc(vect_type);
        const auto field3 = data.gfp(vect_type, alloc2, 0);
        const auto field4 = data.gfp(vect_type, alloc2, 1);
        const auto field5 = data.gfp(vect_type, alloc2, 2);
        data.store(field3, fn(1));
        data.store(field4, fn(2));
        data.store(field5, fn(3));

        const auto cont = data.create_basic_block();
        const auto call = data.call(ext_proto, cont, {alloc1, alloc2});
        data.switch_block(cont);
        data.ret(call);
    }

    return builder.build();
}

TEST(StructArg, pass_external_two_struct_by_value) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 27},
    };

    const std::unordered_map<std::string, std::size_t> external_symbols{
        {"sum", reinterpret_cast<std::size_t>(&sum_ints3)}
    };

    const auto buffer = jit_compile_and_assembly(external_symbols, struct_arg_external3(SignedIntegerType::i64(), Value::i64), asm_size, true);
    const auto sum_fields = buffer.code_start_as<int64_t()>("sum_fields").value();
    const auto sum = sum_fields();
    ASSERT_EQ(sum, 20 + 30 + 40 + 1 + 2 + 3);
}

TEST(StructArg, pass_external_two_struct_by_value_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"sum_fields", 33},
    };

    const std::unordered_map<std::string, std::size_t> external_symbols{
        {"sum", reinterpret_cast<std::size_t>(&sum_floats3)}
    };

    const auto buffer = jit_compile_and_assembly(external_symbols, struct_arg_external3(FloatingPointType::f64(), Value::f64), asm_size, true);
    const auto sum_fields = buffer.code_start_as<double()>("sum_fields").value();
    const auto sum = sum_fields();
    ASSERT_EQ(sum, 20 + 30 + 40 + 1 + 2 + 3);
}

template<typename Fn, typename Fn1>
static Module make_point2(const PrimitiveType* ty1, Fn&& fn1, const PrimitiveType* ty2, Fn1&& fn2) {
    ModuleBuilder builder;
    {
        const auto tuple = TupleType::tuple(ty1, ty2);
        const auto prototype = builder.add_function_prototype(tuple, {}, "make_point2", FunctionBind::DEFAULT);
        const auto data = builder.make_function_builder(prototype).value();
        data.ret(fn1(1234), fn2(5678));
    }

    return builder.build();
}

TEST(ReturnStruct, return_struct_by_value_i64_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"make_point2", 3},
    };

    const auto module = make_point2(SignedIntegerType::i64(), Value::i64, SignedIntegerType::i64(), Value::i64);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size);
    const auto make_point2 = buffer.code_start_as<Point2()>("make_point2").value();
    const auto p = make_point2();
    ASSERT_EQ(p.x, 1234);
    ASSERT_EQ(p.y, 5678);
}

TEST(ReturnStruct, return_struct_by_value_f64_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"make_point2", 3},
    };

    const auto module = make_point2(FloatingPointType::f64(), Value::f64, FloatingPointType::f64(), Value::f64);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size);
    const auto make_point2 = buffer.code_start_as<Point2F64()>("make_point2").value();
    const auto p = make_point2();
    ASSERT_EQ(p.x, 1234);
    ASSERT_EQ(p.y, 5678);
}

TEST(ReturnStruct, return_struct_by_value_i64_f64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"make_point2", 3},
    };

    const auto module = make_point2(SignedIntegerType::i64(), Value::i64, FloatingPointType::f64(), Value::f64);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size);
    const auto make_point2 = buffer.code_start_as<std::pair<std::int64_t, double>()>("make_point2").value();
    const auto p = make_point2();
    ASSERT_EQ(p.first, 1234);
    ASSERT_EQ(p.second, 5678);
}

TEST(ReturnStruct, return_struct_by_value_f64_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size{
        {"make_point2", 3},
    };

    const auto module = make_point2(FloatingPointType::f64(), Value::f64, SignedIntegerType::i64(), Value::i64);
    const auto buffer = jit_compile_and_assembly({}, module, asm_size);
    const auto make_point2 = buffer.code_start_as<std::pair<double, std::int64_t>()>("make_point2").value();
    const auto p = make_point2();
    ASSERT_EQ(p.first, 1234);
    ASSERT_EQ(p.second, 5678);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}