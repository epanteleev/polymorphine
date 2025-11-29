#pragma once

class GPUnaryXmmOutVisitor {
public:
    virtual ~GPUnaryXmmOutVisitor() = default;

    virtual void emit(aasm::XmmReg out, aasm::GPReg in) = 0;
    virtual void emit(aasm::XmmReg out, const aasm::Address& in) = 0;
    virtual void emit(const aasm::Address& out, aasm::GPReg in) = 0;
    virtual void emit(const aasm::Address& out, const aasm::Address& in) = 0;
    virtual void emit(aasm::XmmReg out, std::int64_t in) = 0;
    virtual void emit(const aasm::Address& out, std::int64_t in) = 0;

    template<std::derived_from<GPUnaryXmmOutVisitor> Vis>
    static void dispatch(Vis &visitor, const XVReg &out, const GPOp &in) {
        const auto v = [&]<typename T>(const T& reg) {
            const auto v1 = [&]<typename U>(const U& in_val) {
                visitor.emit(reg, in_val);
            };
            in.visit(v1);
        };

        out.visit(v);
    }
};