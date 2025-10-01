#include <ostream>

#include "XOp.h"
#include "base/Constant.h"


std::ostream & operator<<(std::ostream &os, const XOp &reg) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, aasm::XmmRegister>) {
            os << val.name(cst::QWORD_SIZE*2);

        } else if constexpr (std::is_same_v<T, aasm::Address>) {
            os << val;

        } else if constexpr (std::is_integral_v<T>) {
            os << "0x" << std::hex << val;

        } else {
            static_assert(false, "Unsupported type in XOp variant");
        }
    };

    std::visit(visitor, reg.m_reg);
    return os;
}