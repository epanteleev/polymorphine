#include "Type.h"

#include <iostream>

namespace {
    class Printer final: type::Visitor {
    public:
        explicit Printer(std::ostream &os): os(os) {}

        void do_print(Type* type) {
            type->visit(*this);
        }

    private:
        void accept(SignedIntegerType *type) override {
            os << 'i' << type->size_of() * 8;
        }

        void accept(UnsignedIntegerType *type) override {
            os << 'u' << type->size_of() * 8;
        }

        void accept(FloatingPointType *type) override {
            os << 'f' << type->size_of() * 8;
        }

        std::ostream &os;
    };
}

void Type::print(std::ostream &os) const {
    Printer p(os);
    p.do_print(const_cast<Type *>(this));
}
