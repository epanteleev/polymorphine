#pragma once

#include "Value.h"

namespace impls {
    inline bool constant(const Value* value) {
        return value->is<double>() || value->is<std::int64_t>() || value->is<std::uint64_t>();
    }
}

using ValueMatcher = bool(*)(const Value*);

consteval ValueMatcher constant() {
    return impls::constant;
}
