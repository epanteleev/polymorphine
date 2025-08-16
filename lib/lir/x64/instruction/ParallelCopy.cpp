#include "ParallelCopy.h"

void ParallelCopy::visit(LIRVisitor &visitor) {
    const auto lir_vals = to_lir_vals_only(inputs());
    visitor.parallel_copy(def(0), lir_vals);
}