#include <iostream>
#include <memory>

#include "lib/ir.h"


int main() {
    auto type = std::make_unique<SignedIntegerType>(4);
    if (type->isa(signedType() && i32())) {
        std::cout << "Type is a signed integer type." << std::endl;
    }

    FunctionPrototype proto(SignedIntegerType::i32(), {SignedIntegerType::i32()}, "main");
    ArgumentValue arg(1, SignedIntegerType::i32());
    FunctionData fd(0, std::move(proto), {std::move(arg)});

    auto bb = fd.begin();
    auto arg0 = fd.arg(0);
    auto add = bb->push_back(Binary::add(arg0, Value::i32(2)));
    auto sub = bb->push_back(Binary::sub(Value::i32(5), add));


    fd.print(std::cout);
    return 0;
}
