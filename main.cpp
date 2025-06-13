#include <iostream>

#include "builder/FunctionBuilder.hpp"
#include "lib/ir.h"


std::unique_ptr<FunctionData> fib() {
    FunctionPrototype prototype(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "fib");

    ArgumentValue arg(0, SignedIntegerType::i32());
    auto data = FunctionBuilder::make(0, std::move(prototype), { std::move(arg) });
    auto arg0 = data.arg(0);
    //auto ret_addr = data.alloc(SignedIntegerType::i32());
    auto n_addr = data.alloc(SignedIntegerType::i32());
    data.store(n_addr, arg0);
    return data.build();
}

int main() {
    constexpr auto type = SignedIntegerType::i32();
    if (type->isa(signedType() && i32())) {
        std::cout << "Type is a signed integer type." << std::endl;
    }

    FunctionPrototype proto(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "main");
    ArgumentValue arg(1, SignedIntegerType::i32());
    auto fd = fib();

    fd->print(std::cout);
    return 0;
}
