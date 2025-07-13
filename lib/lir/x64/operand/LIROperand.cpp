#include "LIROperand.h"
#include <ostream>

std::ostream & operator<<(std::ostream &os, const LIROperand &op) noexcept {
    auto cst_opt = op.cst();
    if (cst_opt.has_value()) {
        os << cst_opt.value();
        return os;
    }

    auto vreg_opt = op.vreg();
    if (vreg_opt.has_value()) {
        os << vreg_opt.value();
        return os;
    }

    die("wrong variant");
}
