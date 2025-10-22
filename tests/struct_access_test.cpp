#include <gtest/gtest.h>
#include <climits>

#include "helpers/Jit.h"
#include "mir/mir.h"

static Module point_struct() {
    ModuleBuilder builder;
    auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
    {
        const auto prototype = builder.add_function_prototype(VoidType::type(), {PointerType::ptr()}, "init_point", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(point_type, arg0, 0);
        const auto field1 = data.gfp(point_type, arg0, 1);
        data.store(field0, Value::i32(-42));
        data.store(field1, Value::u64(static_cast<std::int64_t>(INT_MAX)+2));
        data.ret();
    }
    {
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "sum_fields", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(point_type, arg0, 0);
        const auto field1 = data.gfp(point_type, arg0, 1);
        const auto val0 = data.load(SignedIntegerType::i32(), field0);
        const auto val1 = data.load(UnsignedIntegerType::u64(), field1);
        const auto val0_u64 = data.sext(SignedIntegerType::i64(), val0);
        const auto val1_i64 = data.bitcast(SignedIntegerType::i64(), val1);
        const auto sum = data.add(val0_u64, val1_i64);
        data.ret(sum);
    }

    return builder.build();
}

struct Point {
    int32_t x;
    uint64_t y;
};

TEST(StructAccess, init) {
    const auto buffer = jit_compile_and_assembly(point_struct(), true);
    const auto init = buffer.code_start_as<void(Point*)>("init_point").value();
    const auto sum_fields = buffer.code_start_as<int64_t(Point*)>("sum_fields").value();
    Point p{};
    init(&p);
    ASSERT_EQ(p.x, -42);
    ASSERT_EQ(p.y, static_cast<std::int64_t>(INT_MAX)+2);
    const auto sum = sum_fields(&p);
    ASSERT_EQ(sum, -42 + static_cast<std::int64_t>(INT_MAX)+2);
}

static Module vec_struct() {
    ModuleBuilder builder;
    auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
    auto vec_type = builder.add_struct_type("Vec", {point_type, point_type});
    {
        const auto prototype = builder.add_function_prototype(VoidType::type(), {PointerType::ptr()}, "init_vec", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vec_type, arg0, 0);
        const auto field1 = data.gfp(vec_type, arg0, 1);
        const auto field0_0 = data.gfp(point_type, field0, 0);
        const auto field0_1 = data.gfp(point_type, field0, 1);
        const auto field1_0 = data.gfp(point_type, field1, 0);
        const auto field1_1 = data.gfp(point_type, field1, 1);
        data.store(field0_0, Value::i32(-42));
        data.store(field0_1, Value::u64(static_cast<std::int64_t>(INT_MAX)+2));
        data.store(field1_0, Value::i32(100));
        data.store(field1_1, Value::u64(static_cast<std::int64_t>(INT_MAX)+3));
        data.ret();
    }
    {
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "sum_fields", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(std::move(prototype)).value();
        const auto arg0 = data.arg(0);
        const auto field0 = data.gfp(vec_type, arg0, 0);
        const auto field1 = data.gfp(vec_type, arg0, 1);
        const auto field0_0 = data.gfp(point_type, field0, 0);
        const auto field0_1 = data.gfp(point_type, field0, 1);
        const auto field1_0 = data.gfp(point_type, field1, 0);
        const auto field1_1 = data.gfp(point_type, field1, 1);
        const auto val0_0 = data.load(SignedIntegerType::i32(), field0_0);
        const auto val0_1 = data.load(UnsignedIntegerType::u64(), field0_1);
        const auto val1_0 = data.load(SignedIntegerType::i32(), field1_0);
        const auto val1_1 = data.load(UnsignedIntegerType::u64(), field1_1);
        const auto val0_0_u64 = data.sext(SignedIntegerType::i64(), val0_0);
        const auto val0_1_i64 = data.bitcast(SignedIntegerType::i64(), val0_1);
        const auto val1_0_u64 = data.sext(SignedIntegerType::i64(), val1_0);
        const auto val1_1_i64 = data.bitcast(SignedIntegerType::i64(), val1_1);
        const auto sum = data.add(val0_0_u64, val1_0_u64);
        const auto sum2 = data.add(val0_1_i64, val1_1_i64);
        const auto total = data.add(sum, sum2);
        data.ret(total);
    }

    return builder.build();
}

struct Vec {
    Point p0;
    Point p1;
};

TEST(StructAccess, nested_stackalloc) {
    const auto buffer = jit_compile_and_assembly(vec_struct(), true);
    const auto init = buffer.code_start_as<void(Point*)>("init_vec").value();
    const auto sum_fields = buffer.code_start_as<int64_t(Point*)>("sum_fields").value();

    Vec v{};
    init(reinterpret_cast<Point*>(&v));
    ASSERT_EQ(v.p0.x, -42);
    ASSERT_EQ(v.p0.y, static_cast<std::int64_t>(INT_MAX)+2);
    ASSERT_EQ(v.p1.x, 100);
    ASSERT_EQ(v.p1.y, static_cast<std::int64_t>(INT_MAX)+3);
    const auto sum = sum_fields(reinterpret_cast<Point*>(&v));
    ASSERT_EQ(sum, -42 + static_cast<std::int64_t>(INT_MAX)+2 + 100 + static_cast<std::int64_t>(INT_MAX)+3);
}

static Module struct_stackalloc() {
    ModuleBuilder builder;
    {
        auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i32(), UnsignedIntegerType::u64()});
        const auto prototype = builder.add_function_prototype(SignedIntegerType::i64(), {}, "make_point", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto alloca = data.alloc(point_type);
        const auto field0 = data.gfp(point_type, alloca, 0);
        const auto field1 = data.gfp(point_type, alloca, 1);
        data.store(field0, Value::i32(-42));
        data.store(field1, Value::u64(static_cast<std::int64_t>(INT_MAX)+2));
        const auto val0 = data.load(SignedIntegerType::i32(), field0);
        const auto val1 = data.load(UnsignedIntegerType::u64(), field1);
        const auto val0_u64 = data.sext(SignedIntegerType::i64(), val0);
        const auto val1_i64 = data.bitcast(SignedIntegerType::i64(), val1);
        const auto sum = data.add(val0_u64, val1_i64);
        data.ret(sum);
    }

    return builder.build();
}

TEST(StructAlloc, stack_alloc) {
    static std::unordered_map<std::string, std::size_t> asm_size {
        {"make_point", 12},
    };

    const auto buffer = jit_compile_and_assembly({}, struct_stackalloc(), asm_size, true);
    const auto make_point = buffer.code_start_as<int64_t()>("make_point").value();
    const auto sum = make_point();
    ASSERT_EQ(sum, static_cast<std::int64_t>(INT_MAX)-42+2);
}

static Module escaped_struct_stackalloc() {
    ModuleBuilder builder;
    {
        const auto sum_prototype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "sum_point", FunctionBind::EXTERN);
        auto point_type = builder.add_struct_type("Point", {SignedIntegerType::i64(), SignedIntegerType::i64()});
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "make_point", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto alloca = data.alloc(point_type);
        const auto field0 = data.gfp(point_type, alloca, 0);
        const auto field1 = data.gfp(point_type, alloca, 1);
        data.store(field0, Value::i64(-42));
        data.store(field1, Value::u64(static_cast<std::int64_t>(INT_MAX)+2));
        const auto cont = data.create_basic_block();
        const auto sum = data.call(sum_prototype, cont, {alloca});
        data.switch_block(cont);
        data.ret(sum);
    }

    return builder.build();
}

struct Point2 {
    std::int64_t x;
    std::int64_t y;
};

static std::int64_t sum_point(Point2* p) {
    return p->x + p->y;
}

TEST(StructAlloc, escaped_stack_alloc) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"make_point", 10},
    };

    const std::unordered_map<std::string, std::size_t> externs {
        {"sum_point", reinterpret_cast<std::size_t>(&sum_point)}
    };

    const auto buffer = jit_compile_and_assembly(externs, escaped_struct_stackalloc(), asm_size);
    const auto make_point = buffer.code_start_as<int64_t()>("make_point").value();
    const auto sum = make_point();
    ASSERT_EQ(sum, static_cast<std::int64_t>(INT_MAX)-42+2);
}

static Module escaped_array_stackalloc() {
    ModuleBuilder builder;
    {
        const auto sum_prototype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "sum_array", FunctionBind::EXTERN);
        const auto array_type = builder.add_array_type(SignedIntegerType::i64(), 3);
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "make_array", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto alloca = data.alloc(array_type);
        const auto field0 = data.gep(array_type, alloca, Value::i64(0));
        const auto field1 = data.gep(array_type, alloca, Value::i64(1));
        const auto field2 = data.gep(array_type, alloca, Value::i64(2));
        data.store(field0, Value::i64(-42));
        data.store(field1, Value::i64(static_cast<std::int64_t>(INT_MAX)+2));
        data.store(field2, Value::i64(100));
        const auto cont = data.create_basic_block();
        const auto sum = data.call(sum_prototype, cont, {alloca});
        data.switch_block(cont);
        data.ret(sum);
    }

    return builder.build();
}

static std::int64_t sum_array(const std::int64_t* arr) {
    return arr[0] + arr[1] + arr[2];
}

TEST(StructAlloc, escaped_array_stack_alloc) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"make_array", 11}, //TODO better codegen. should be 11 instructions.
    };

    const std::unordered_map<std::string, std::size_t> externs {
        {"sum_array", reinterpret_cast<std::size_t>(&sum_array)}
    };

    const auto buffer = jit_compile_and_assembly(externs, escaped_array_stackalloc(), asm_size);
    const auto make_array = buffer.code_start_as<int64_t()>("make_array").value();
    const auto sum = make_array();
    ASSERT_EQ(sum, static_cast<std::int64_t>(-42) + static_cast<std::int64_t>(INT_MAX)+2 + 100);
}

static Module escaped_array_slice() {
    ModuleBuilder builder;
    {
        const auto sum_prototype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr(), SignedIntegerType::i64()}, "sum_array_slice", FunctionBind::EXTERN);
        const auto array_type = builder.add_array_type(SignedIntegerType::i64(), 3);
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "make_array", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto alloca = data.alloc(array_type);
        const auto field0 = data.gep(SignedIntegerType::i64(), alloca, Value::i64(0));
        const auto field1 = data.gep(SignedIntegerType::i64(), alloca, Value::i64(1));
        const auto field2 = data.gep(SignedIntegerType::i64(), alloca, Value::i64(2));
        data.store(field0, Value::i64(-42));
        data.store(field1, Value::i64(static_cast<std::int64_t>(INT_MAX)+2));
        data.store(field2, Value::i64(100));
        const auto cont = data.create_basic_block();
        const auto sum = data.call(sum_prototype, cont, {field1});
        data.switch_block(cont);
        data.ret(sum);
    }

    return builder.build();
}

static std::int64_t sum_array_slice(const std::int64_t* arr) {
    return arr[0] + arr[1];
}

TEST(StructAlloc, escaped_array_slice_stack_alloc) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"make_array", 11}, //TODO better codegen. should be 10 instructions.
    };

    const std::unordered_map<std::string, std::size_t> externs {
        {"sum_array_slice", reinterpret_cast<std::size_t>(&sum_array_slice)}
    };

    const auto buffer = jit_compile_and_assembly(externs, escaped_array_slice(), asm_size, true);
    const auto make_array = buffer.code_start_as<int64_t()>("make_array").value();
    const auto sum = make_array();
    ASSERT_EQ(sum, static_cast<std::int64_t>(INT_MAX)+2 + 100);
}

template<typename Fn>
static Module escaped_struct_field(const IntegerType* ty, Fn&& fn) {
    ModuleBuilder builder;
    {
        const auto sum_prototype = builder.add_function_prototype(SignedIntegerType::i64(), {PointerType::ptr()}, "deref_ptr", FunctionBind::EXTERN);
        auto point_type = builder.add_struct_type("Point", {ty, ty});
        const auto prototype = builder.add_function_prototype(PointerType::ptr(), {}, "make_point", FunctionBind::DEFAULT);
        auto data = builder.make_function_builder(prototype).value();
        const auto alloca = data.alloc(point_type);
        const auto field0 = data.gfp(point_type, alloca, 0);
        const auto field1 = data.gfp(point_type, alloca, 1);

        data.store(field0, fn(-42));
        data.store(field1, fn(static_cast<std::int64_t>(INT_MAX)+2));
        const auto cont = data.create_basic_block();
        const auto deref = data.call(sum_prototype, cont, {field1});
        data.switch_block(cont);

        const auto cont1 = data.create_basic_block();
        const auto deref2 = data.call(sum_prototype, cont1, {field0});
        data.switch_block(cont1);
        const auto total = data.add(deref, deref2);
        data.ret(total);
    }

    return builder.build();
}

template<std::integral T>
static std::int64_t deref_ptr(const T* p) {
    return *p;
}

TEST(StructAlloc, escaped_struct_field_stack_alloc_i64) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"make_point", 18},
    };

    const std::unordered_map<std::string, std::size_t> externs {
        {"deref_ptr", reinterpret_cast<std::size_t>(&deref_ptr<std::int64_t>)}
    };

    const auto buffer = jit_compile_and_assembly(externs, escaped_struct_field(SignedIntegerType::i64(), Value::i64), asm_size, true);
    const auto make_point = buffer.code_start_as<int64_t()>("make_point").value();
    const auto val = make_point();
    ASSERT_EQ(val, -42 + static_cast<std::int64_t>(INT_MAX)+2);
}

TEST(StructAlloc, escaped_struct_field_stack_alloc_u64) {
    const std::unordered_map<std::string, std::size_t> asm_size {
        {"make_point", 18},
    };

    const std::unordered_map<std::string, std::size_t> externs {
        {"deref_ptr", reinterpret_cast<std::size_t>(&deref_ptr<std::uint64_t>)}
    };

    const auto buffer = jit_compile_and_assembly(externs, escaped_struct_field(UnsignedIntegerType::u64(), Value::u64), asm_size, true);
    const auto make_point = buffer.code_start_as<uint64_t()>("make_point").value();
    const auto val = make_point();
    ASSERT_EQ(val, -42 + static_cast<std::int64_t>(INT_MAX)+2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}