#pragma once

#include <unordered_map>
#include <unordered_set>

#include "mir/instruction/ValueInstruction.h"
#include "utility/Error.h"
#include "mir/value/UsedValue.h"


struct HashUsedVal final {
    [[nodiscard]]
    size_t operator()(const UsedValue& val) const noexcept {
        if (val.is<ArgumentValue>()) {
            return val.get<ArgumentValue>()->index();
        }
        if (val.is<ValueInstruction>()) {
            return val.get<ValueInstruction>()->id();
        }

        die("wrong variant");
    }
};

struct UsedValEqualTo final {
    bool operator()(const UsedValue& x, const UsedValue& y) const { return x == y; }
};


template <typename V>
using UsedValueMap = std::unordered_map<UsedValue, V, HashUsedVal, UsedValEqualTo>;

using UsedValueSet = std::unordered_set<UsedValue, HashUsedVal, UsedValEqualTo>;