#pragma once

#include "LIRVal.h"

#include <unordered_map>
#include <unordered_set>


template <typename V>
using LIRValMap = std::unordered_map<LIRVal, V, details::VRegHash, details::VRegEqualTo>;

using LIRValSet = std::unordered_set<LIRVal, details::VRegHash, details::VRegEqualTo>;
