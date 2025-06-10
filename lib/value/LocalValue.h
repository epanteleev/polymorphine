#pragma once

#include <type_traits>

#include "../ir_frwd.h"

template<typename T>
concept LocalValueType = std::is_same_v<T, ValueInstruction> ||
    std::is_same_v<T, ArgumentValue>;
