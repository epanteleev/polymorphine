#include "Type.h"

#include <ostream>

namespace {
    class Printer final: type::Visitor {
    public:
        explicit Printer(std::ostream &os): os(os) {}

        void do_print(const Type* type) {
            const_cast<Type*>(type)->visit(*this);
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

        void accept(PointerType *type) override {
            os << "ptr";
        }

        void accept(FlagType *type) override {
            os << "flag";
        }

        void accept(VoidType *type) override {
            os << "void";
        }

        std::ostream &os;
    };
}

std::ostream & operator<<(std::ostream &os, const Type &obj) {
    Printer p(os);
    p.do_print(&obj);
    return os;
}
