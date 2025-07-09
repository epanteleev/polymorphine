#pragma once

#include <unordered_map>
#include <unordered_set>

#include "mir/instruction/ValueInstruction.h"
#include "utility/Error.h"
#include "mir/value/LocalValue.h"


struct HashLocalVal final {
    [[nodiscard]]
    size_t operator()(const LocalValue& val) const noexcept {
        if (val.is<ArgumentValue>()) {
            return val.get<ArgumentValue>()->index();
        }
        if (val.is<ValueInstruction>()) {
            return val.get<ValueInstruction>()->id();
        }

        die("wrong variant");
    }
};

struct LocalValEqualTo final {
    bool operator()(const LocalValue& x, const LocalValue& y) const { return x == y; }
};


template <typename V>
using LocalValueMap = std::unordered_map<LocalValue, V, HashLocalVal, LocalValEqualTo>;

using LocalValueSet = std::unordered_set<LocalValue, HashLocalVal, LocalValEqualTo>;