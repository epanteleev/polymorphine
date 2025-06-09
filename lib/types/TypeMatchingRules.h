#pragma once

#include "TypeMatcher.h"

namespace impls {
    inline bool signedType(const Type *type) {
        return dynamic_cast<const SignedIntegerType *>(type) != nullptr;
    }

    inline bool i32_impl(const Type *type) {
        if (!signedType(type)) {
            return false;
        }

        const auto as_signed = dynamic_cast<const SignedIntegerType *>(type);
        return as_signed->size_of() == 4;
    }
}

inline TypeMatcher signedType() {
    return TypeMatcher(impls::signedType);
}

inline TypeMatcher i32() {
    return TypeMatcher(impls::i32_impl);
}
