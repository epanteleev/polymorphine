#include "LIROperand.h"
#include <ostream>

std::uint8_t LIROperand::size() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, LirCst> || std::is_same_v<T, LIRVReg>) {
            return val.size();
        } else {
            static_assert(false, "Unsupported type in Value variant");
        }
    };

    return std::visit(visitor, m_operand);
}

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
