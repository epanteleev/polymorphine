#include "GPVReg.h"

std::ostream & operator<<(std::ostream &os, const GPVReg &reg) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, aasm::GPReg>) {
            os << val.name(8);
        } else if constexpr (std::is_same_v<T, aasm::Address>) {
            os << val;
        } else {
            static_assert(false, "Unsupported type in GPVReg variant");
        }
    };

    std::visit(visitor, reg.m_reg);
    return os;
}