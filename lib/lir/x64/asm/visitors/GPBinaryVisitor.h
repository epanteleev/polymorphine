#pragma once

#include "lir/x64/lir_frwd.h"

#include "lir/x64/asm/GPOp.h"

class GPBinaryVisitor {
public:
    virtual ~GPBinaryVisitor() = default;

    virtual void emit(aasm::GPReg out, aasm::GPReg in1, aasm::GPReg in2) = 0;
    virtual void emit(aasm::GPReg out, aasm::GPReg in1, const aasm::Address& in2) = 0;
    virtual void emit(aasm::GPReg out, const aasm::Address& in1, aasm::GPReg in2) = 0;
    virtual void emit(aasm::GPReg out, const aasm::Address& in1, const aasm::Address& in2) = 0;

    virtual void emit(aasm::GPReg out, aasm::GPReg in1, std::int32_t in2) = 0;
    virtual void emit(aasm::GPReg out, std::int32_t in1, aasm::GPReg in2) = 0;
    virtual void emit(aasm::GPReg out, std::int32_t in1, std::int32_t in2) = 0;

    virtual void emit(aasm::GPReg out, std::int32_t in1, const aasm::Address& in2) = 0;
    virtual void emit(aasm::GPReg out, const aasm::Address& in1, std::int32_t in2) = 0;

    virtual void emit(const aasm::Address& out, aasm::GPReg in1, aasm::GPReg in2) = 0;
    virtual void emit(const aasm::Address& out, aasm::GPReg in1, const aasm::Address& in2) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in1, aasm::GPReg in2) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in1, const aasm::Address& in2) = 0;

    virtual void emit(const aasm::Address& out, aasm::GPReg in1, std::int32_t in2) = 0;
    virtual void emit(const aasm::Address& out, std::int32_t in1, std::int32_t in2) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in1, std::int32_t in2) = 0;
    virtual void emit(const aasm::Address& out, std::int32_t in1, const aasm::Address& in2) = 0;
    virtual void emit(const aasm::Address& out, std::int32_t in1, aasm::GPReg in2) = 0;

    template<std::derived_from<GPBinaryVisitor> Vis>
    static void dispatch(Vis &visitor, const GPVReg &out, const GPOp &in1, const GPOp &in2) {
        const auto v1 = [&]<typename T>(const T& reg) {
            const auto v2 = [&]<typename U>(const U& in_val) {
                const auto v3 = [&]<typename V>(const V& in_val2) {
                    visitor.emit(reg, in_val, in_val2);
                };

                in2.visit(v3);
            };
            in1.visit(v2);
        };

        out.visit(v1);
    }
};
