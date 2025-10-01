#pragma once

#include "lir/x64/asm/operand/XOp.h"
#include "lir/x64/asm/operand/XVReg.h"


class XUnaryOutVisitor {
public:
    virtual ~XUnaryOutVisitor() = default;

    virtual void emit(aasm::XmmReg out, aasm::XmmReg in) = 0;
    virtual void emit(aasm::XmmReg out, const aasm::Address& in) = 0;
    virtual void emit(const aasm::Address& out, aasm::XmmReg in) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in) = 0;
    virtual void emit(aasm::XmmReg out, std::int64_t in) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in) = 0;

    template<std::derived_from<XUnaryOutVisitor> Vis>
    static void dispatch(Vis &visitor, const XVReg &out, const XOp &in) {
        const auto v = [&]<typename T>(const T& reg) {
            const auto v1 = [&]<typename U>(const U& in_val) {
                visitor.emit(reg, in_val);
            };
            in.visit(v1);
        };

        out.visit(v);
    }
};
