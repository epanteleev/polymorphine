#include "Type.h"

#include <iostream>


SignedIntegerType * SignedIntegerType::i8() noexcept {
    static SignedIntegerType i8_instance(1);
    return &i8_instance;
}

SignedIntegerType *SignedIntegerType::i16() noexcept {
    static SignedIntegerType i16_instance(2);
    return &i16_instance;
}

SignedIntegerType *SignedIntegerType::i32() noexcept {
    static SignedIntegerType i32_instance(4);
    return &i32_instance;
}

SignedIntegerType * SignedIntegerType::i64() noexcept {
    static SignedIntegerType i8_instance(8);
    return &i8_instance;
}

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

