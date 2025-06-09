#include <iostream>
#include <memory>

#include "lib/BasicBlock.h"
#include "lib/types/Type.h"
#include "lib/types/TypeMatchingRules.h"


int main() {
    auto type = std::make_unique<SignedIntegerType>(4);
    if (type->isa(signedType() && i32())) {
        std::cout << "Type is a signed integer type." << std::endl;
    }

    BasicBlock bb;
    const auto v = bb.push_back(BinaryOp::Add, Value::i32(3), Value::i32(4));
    v->print(std::cout);


    return 0;
}
