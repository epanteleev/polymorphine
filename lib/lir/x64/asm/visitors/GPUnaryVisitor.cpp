#include "GPUnaryVisitor.h"

#include "lir/x64/asm/GPVReg.h"

template<typename T>
void dispatch_helper(GPUnaryVisitor &visitor, const aasm::GPReg &out, const T &in) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.rr(out, in);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.rm(out, in);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.ri(out, in);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

template<typename T>
void dispatch_helper(GPUnaryVisitor &visitor, const aasm::Address &out, const T &in) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.mr(out, in);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.mm(out, in);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.mi(out, in);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

void GPUnaryVisitor::dispatch(GPUnaryVisitor &visitor, const GPVReg &out, const GPOp &in) {
    const auto v = [&]<typename T>(const T& reg) {
        const auto v1 = [&]<typename U>(const U& in_val) {
            dispatch_helper(visitor, reg, in_val);
        };
        in.visit(v1);
    };

    out.visit(v);
}
