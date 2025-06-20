#pragma once

#include <unordered_set>
#include "FunctionData.h"

namespace fn {
    struct Hash final {
        std::size_t operator()(const FunctionData& s) const noexcept {
            constexpr std::hash<std::string_view> hash;
            return hash(s.name());
        }
    };

    struct EqualTo final {
        bool operator()(const FunctionData& _x, const FunctionData& _y) const {
            if (&_x == &_y) {
                return true;
            }

            return _x.name() == _y.name();
        }
    };

    using FunctionSet = std::unordered_set<FunctionData, Hash, EqualTo>;
}