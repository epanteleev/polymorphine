#pragma once

#include "lir/x64/asm/operand/GPOp.h"

class GPUnaryVisitor {
public:
    virtual ~GPUnaryVisitor() = default;

    virtual void emit(aasm::GPReg in0, aasm::GPReg in) = 0;
    virtual void emit(aasm::GPReg in0, const aasm::Address& in) = 0;
    virtual void emit(const aasm::Address& in0, aasm::GPReg in) = 0;
    virtual void emit(const aasm::Address& in0, const aasm::Address& in) = 0;
    virtual void emit(aasm::GPReg in0, std::int64_t in) = 0;
    virtual void emit(const aasm::Address& in0, std::int64_t in) = 0;
    virtual void emit(std::int64_t in1, std::int64_t in2) = 0;
    virtual void emit(std::int64_t in1, aasm::GPReg in2) = 0;
    virtual void emit(std::int64_t in1, const aasm::Address& in2) = 0;

    template<std::derived_from<GPUnaryVisitor> Vis>
    static void dispatch(Vis& visitor, const GPOp& in0, const GPOp& in) {
        const auto v = [&]<typename T>(const T& reg) {
            const auto v1 = [&]<typename U>(const U& in_val) {
                visitor.emit(reg, in_val);
            };
            in.visit(v1);
        };

        in0.visit(v);
    }
};
