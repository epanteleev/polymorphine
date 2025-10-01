#include "XVReg.h"

#include "base/Constant.h"


std::ostream & operator<<(std::ostream &os, const XVReg &reg) noexcept {
    const auto visitor = [&]<typename T>(const T &val) -> std::ostream& {
        if constexpr (std::is_same_v<T, aasm::XmmReg>) {
            return os << val.name(cst::QWORD_SIZE*2);

        } else if constexpr (std::is_same_v<T, aasm::Address>) {
            return os << val;

        } else {
            static_assert(false, "Unsupported type in GPVReg variant");
            std::unreachable();
        }
    };

    return std::visit(visitor, reg.m_reg);
}