#include "Reg.h"
#include "base/Constant.h"

namespace aasm {
    std::ostream & operator<<(std::ostream &os, const Reg &reg) {
        const auto vis = [&]<typename T>(const T& r) -> std::ostream& {
            if constexpr (std::is_same_v<T, GPReg>) {
                return os << r.name(cst::QWORD_SIZE);

            } else if constexpr (std::is_same_v<T, XmmReg>) {
                return os << r.name(cst::QWORD_SIZE*2);

            } else {
                static_assert(false);
                std::unreachable();
            }
        };

        return std::visit(vis, reg.m_reg);
    }
}