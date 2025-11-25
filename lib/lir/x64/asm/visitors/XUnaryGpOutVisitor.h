#pragma once

class XUnaryGpOutVisitor {
public:
    virtual ~XUnaryGpOutVisitor() = default;

    virtual void emit(aasm::GPReg out, aasm::XmmReg in) = 0;
    virtual void emit(aasm::GPReg out, const aasm::Address& in) = 0;
    virtual void emit(aasm::GPReg out, std::int64_t in) = 0;
    virtual void emit(const aasm::Address& out, aasm::XmmReg in) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in) = 0;

    template<std::derived_from<XUnaryGpOutVisitor> Vis>
    static void dispatch(Vis &visitor, const GPVReg &out, const XOp &in) {
        const auto v = [&]<typename T>(const T& reg) {
            const auto v1 = [&]<typename U>(const U& in_val) {
                visitor.emit(reg, in_val);
            };
            in.visit(v1);
        };

        out.visit(v);
    }
};