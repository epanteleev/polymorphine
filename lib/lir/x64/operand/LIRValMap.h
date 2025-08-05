#pragma once

#include "LIRVal.h"

#include <unordered_map>
#include <unordered_set>


template <typename V>
using LIRValMap = std::unordered_map<LIRVal, V, details::LIRValHash, details::LIRValEqualTo>;

using LIRValSet = std::unordered_set<LIRVal, details::LIRValHash, details::LIRValEqualTo>;
