#pragma once

#include <unordered_map>
#include <unordered_set>

#include "GPVReg.h"

template <typename V>
using GPVRegMap = std::unordered_map<GPVReg, V, details::GPVRegHash, details::GPVRegEqualTo>;

using GPVRegSet = std::unordered_set<GPVReg, details::GPVRegHash, details::GPVRegEqualTo>;