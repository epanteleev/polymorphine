#include <iostream>
#include <memory>

#include "lib/Instruction.h"
#include "lib/types/Type.h"
#include "lib/types/TypeMatchingRules.h"
#include "lib/utlility/OrderedSet.h"


int main() {
    auto type = std::make_unique<SignedIntegerType>(4);
    if (type->isa(signedType() && i32())) {
        std::cout << "Type is a signed integer type." << std::endl;
    }

    OrderedSet<Instruction> orderedSet;
    auto instruction = orderedSet.push_back<BinaryInstruction>(nullptr, BinaryOp::Add);

    return 0;
}
