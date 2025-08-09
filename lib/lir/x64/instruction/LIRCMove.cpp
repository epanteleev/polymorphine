#include "LIRCMove.h"

void LIRCMove::visit(LIRVisitor &visitor) {
    visitor.cmov_i(m_cond_type, def(0), in(0), in(1));
}