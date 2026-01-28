#pragma once

#include "IntegerType.h"

namespace impls {
    inline bool gp_type(const Type *type) noexcept {
        const auto int_type = IntegerType::cast(type);
        if (int_type != nullptr) {
            return true;
        }

        return PointerType::cast(type) != nullptr;
    }
}

consteval auto gp_type() noexcept {
    return impls::gp_type;
}