#pragma once

#include "mir/mir_frwd.h"

namespace type {
    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void accept(SignedIntegerType *type) = 0;
        virtual void accept(UnsignedIntegerType *type) = 0;
        virtual void accept(FloatingPointType *type) = 0;
        virtual void accept(PointerType *ptr) = 0;
        virtual void accept(FlagType *type) = 0;
        virtual void accept(VoidType *type) = 0;
        virtual void accept(StructType *type) = 0;
        virtual void accept(ArrayType *type) = 0;
        virtual void accept(TupleType *type) = 0;
    };
}
