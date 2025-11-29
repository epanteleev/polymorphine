#pragma once

class XBinaryAddrVisitor {
public:
    virtual ~XBinaryAddrVisitor() = default;

    virtual void emit(const aasm::Address& out, aasm::GPReg in1, aasm::XmmReg in2) = 0;
    virtual void emit(const aasm::Address& out, aasm::GPReg in1, const aasm::Address& in2) = 0;
    virtual void emit(const aasm::Address& out, aasm::GPReg in1, std::int64_t in2) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in1, aasm::XmmReg in2) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in1, const aasm::Address& in2) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in1, std::int64_t in2) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in1, aasm::XmmReg in2) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in1, std::int64_t in2) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in1, const aasm::Address& in2) = 0;

    template<std::derived_from<XBinaryAddrVisitor> Vis>
    static void dispatch(Vis &visitor, const aasm::Address &out, const GPOp &in1, const XOp &in2) {
        const auto v2 = [&]<typename U>(const U& in_val) {
            const auto v3 = [&]<typename V>(const V& in_val2) {
                visitor.emit(out, in_val, in_val2);
            };

            in2.visit(v3);
        };
        in1.visit(v2);
    }
};