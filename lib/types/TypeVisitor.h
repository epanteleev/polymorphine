#pragma once

#include "ir_frwd.h"

namespace type {
    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void accept(SignedIntegerType *type) = 0;
        virtual void accept(UnsignedIntegerType *type) = 0;
        virtual void accept(FloatingPointType *type) = 0;
        virtual void accept(PointerType* ptr) = 0;
        virtual void accept(FlagType *type) = 0;
    };
}
