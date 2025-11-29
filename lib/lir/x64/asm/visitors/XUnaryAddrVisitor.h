#pragma once

class XUnaryAddrVisitor {
public:
    virtual ~XUnaryAddrVisitor() = default;

    virtual void emit(const aasm::Address& out, aasm::XmmReg in) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in) = 0;

    template<std::derived_from<XUnaryAddrVisitor> Vis>
    static void dispatch(Vis &visitor, const aasm::Address &out, const XOp &in) {
        const auto v1 = [&]<typename U>(const U& in_val) {
            visitor.emit(out, in_val);
        };

        in.visit(v1);
    }
};