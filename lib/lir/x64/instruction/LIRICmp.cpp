#include "LIRICmp.h"

void LIRICmp::visit(LIRVisitor &visitor) {
    visitor.cmp_i(in(0), in(1));
}