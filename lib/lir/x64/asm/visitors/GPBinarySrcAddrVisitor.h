#pragma once

#include "asm/x64/address/Address.h"
#include "../operand/GPOp.h"

class GPBinarySrcAddrVisitor {
public:
    virtual ~GPBinarySrcAddrVisitor() = default;

    virtual void emit(aasm::GPReg out, aasm::GPReg in, const aasm::Address& src) = 0;
    virtual void emit(aasm::GPReg out, const aasm::Address& in, const aasm::Address& src) = 0;
    virtual void emit(aasm::GPReg out, std::int64_t in, const aasm::Address& src) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in, const aasm::Address& src) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in, const aasm::Address& src) = 0;
    virtual void emit(const aasm::Address& out, aasm::GPReg in, const aasm::Address& src) = 0;

    template<std::derived_from<GPBinarySrcAddrVisitor> Vis>
    static void dispatch(Vis &visitor, const GPVReg &out, const GPOp &in, const aasm::Address &src) {
        const auto v2 = [&]<typename U>(const U& in_val) {
            const auto v3 = [&]<typename V>(const V& in_val2) {
                visitor.emit(in_val2, in_val, src);
            };

            out.visit(v3);
        };
        in.visit(v2);
    }
};
