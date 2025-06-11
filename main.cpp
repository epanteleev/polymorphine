#include <iostream>

#include "lib/ir.h"


FunctionData fib() {
    FunctionPrototype prototype(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "fib");

    ArgumentValue arg(0, SignedIntegerType::i32());
    FunctionData fn(0, std::move(prototype), {std::move(arg)});
    auto bb = fn.begin();
    const auto arg0 = fn.arg(0);

    return fn;
}

int main() {
    constexpr auto type = SignedIntegerType::i32();
    constexpr auto value = Value::i32(2);
    if (type->isa(signedType() && i32())) {
        std::cout << "Type is a signed integer type." << std::endl;
    }

    FunctionPrototype proto(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "main");
    ArgumentValue arg(1, SignedIntegerType::i32());
    FunctionData fd(0, std::move(proto), {std::move(arg)});

    auto bb = fd.begin();
    const auto arg0 = fd.arg(0);
    auto add = bb->push_back(Binary::add(arg0, value));
    auto sub = bb->push_back(Binary::sub(Value::i32(5), add));
    bb->push_back(ReturnValue::ret(sub));

    fd.print(std::cout);
    return 0;
}
