#include "LIRSetCC.h"
#include <ostream>
#include <utility>

void LIRSetCC::visit(LIRVisitor &visitor) {
    visitor.setcc_i(def(0), m_cond_type);
}
