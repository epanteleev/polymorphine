#pragma once

#include "LIRVReg.h"

#include <unordered_map>
#include <unordered_set>

namespace details {
    struct VRegHash final {
        [[nodiscard]]
        std::size_t operator()(const LIRVReg& val) const noexcept {
            return val.size() ^ val.index(); // TODO: better hash func
        }
    };

    struct VRegEqualTo final {
        bool operator()(const LIRVReg& x, const LIRVReg& y) const { return x == y; }
    };
}


template <typename V>
using LIRVRegMap = std::unordered_map<LIRVReg, V, details::VRegHash, details::VRegEqualTo>;

using LIRVRegSet = std::unordered_set<LIRVReg, details::VRegHash, details::VRegEqualTo>;
