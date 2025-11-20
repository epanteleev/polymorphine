#include "LIRFCmp.h"

void LIRFCmp::visit(LIRVisitor &visitor) {
    visitor.cmp_f(m_cmp_type, in(0), in(1));
}