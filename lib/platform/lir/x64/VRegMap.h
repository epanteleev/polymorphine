#pragma once

#include "Vreg.h"

#include <unordered_map>
#include <unordered_set>

struct VRegHash final {
    [[nodiscard]]
    std::size_t operator()(const VReg& val) const noexcept {
        return val.size() ^ val.index(); // TODO: better hash func
    }
};

struct VRegEqualTo final {
    bool operator()(const VReg& x, const VReg& y) const { return x == y; }
};

template <typename V>
using VRegMap = std::unordered_map<VReg, V, VRegHash, VRegEqualTo>;

using VRegSet = std::unordered_set<VReg, VRegHash, VRegEqualTo>;
