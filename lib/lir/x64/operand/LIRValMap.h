#pragma once

#include "LIRVal.h"

#include <unordered_map>
#include <unordered_set>

namespace details {
    struct VRegHash final {
        [[nodiscard]]
        std::size_t operator()(const LIRVal& val) const noexcept {
            return val.size() ^ val.index(); // TODO: better hash func
        }
    };

    struct VRegEqualTo final {
        bool operator()(const LIRVal& x, const LIRVal& y) const { return x == y; }
    };
}


template <typename V>
using LIRValMap = std::unordered_map<LIRVal, V, details::VRegHash, details::VRegEqualTo>;

using LIRValSet = std::unordered_set<LIRVal, details::VRegHash, details::VRegEqualTo>;
