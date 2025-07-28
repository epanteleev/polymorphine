#pragma once

#include "lir/x64/asm/GPOp.h"

class GPUnaryAddrVisitor {
public:
    virtual ~GPUnaryAddrVisitor() = default;

    virtual void emit(const aasm::Address& out, aasm::GPReg in) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in) = 0;

    template<std::derived_from<GPUnaryAddrVisitor> Vis>
    static void dispatch(Vis &visitor, const aasm::Address &out, const GPOp &in) {
        const auto v1 = [&]<typename U>(const U& in_val) {
            visitor.emit(out, in_val);
        };

        in.visit(v1);
    }
};