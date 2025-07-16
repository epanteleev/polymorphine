#include "GPBinaryVisitor.h"

#include "lir/x64/asm/GPOp.h"

#include "asm/Address.h"

template<typename T>
void dispatch_helper(GPBinaryVisitor &visitor, const aasm::GPReg &out, const aasm::GPReg &in1, const T &in2) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.rrr(out, in1, in2);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.rrm(out, in1, in2);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.rri(out, in1, in2);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

template<typename T>
void dispatch_helper(GPBinaryVisitor &visitor, const aasm::GPReg &out, std::int32_t in1, const T &in2) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.rir(out, in1, in2);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.rim(out, in1, in2);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.rii(out, in1, in2);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

template<typename T>
void dispatch_helper(GPBinaryVisitor &visitor, const aasm::GPReg &out, const aasm::Address &in1, const T &in2) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.rmr(out, in1, in2);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.rmm(out, in1, in2);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.rmi(out, in1, in2);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

template<typename T>
void dispatch_helper(GPBinaryVisitor &visitor, const aasm::Address &out, const aasm::GPReg &in1, const T &in2) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.mrr(out, in1, in2);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.mrm(out, in1, in2);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.mri(out, in1, in2);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

template<typename T>
void dispatch_helper(GPBinaryVisitor &visitor, const aasm::Address &out, const aasm::Address &in1, const T &in2) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.mmr(out, in1, in2);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.mmm(out, in1, in2);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.mmi(out, in1, in2);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

template<typename T>
void dispatch_helper(GPBinaryVisitor &visitor, const aasm::Address &out, std::int32_t in1, const T &in2) {
    if constexpr (std::is_same_v<T, aasm::GPReg>) {
        visitor.mir(out, in1, in2);
    } else if constexpr (std::is_same_v<T, aasm::Address>) {
        visitor.mim(out, in1, in2);
    } else if constexpr (std::is_integral_v<T>) {
        visitor.mii(out, in1, in2);
    } else {
        static_assert(false, "Unsupported type for GPUnaryVisitor dispatch");
    }
}

void GPBinaryVisitor::dispatch(GPBinaryVisitor &visitor, const GPVReg &out, const GPOp &in1, const GPOp &in2) {
    const auto v1 = [&]<typename T>(const T& reg) {
        const auto v2 = [&]<typename U>(const U& in_val) {
            const auto v3 = [&]<typename V>(const V& in_val2) {
                dispatch_helper(visitor, reg, in_val, in_val2);
            };

            in2.visit(v3);
        };
        in1.visit(v2);
    };

    out.visit(v1);
}