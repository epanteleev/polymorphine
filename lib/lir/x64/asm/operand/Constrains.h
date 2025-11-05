#pragma once
#include <type_traits>

#include "asm/x64/asm.h"

template<typename T>
concept GPOpVariant = std::is_same_v<T, aasm::GPReg> ||
    std::is_same_v<T, aasm::Address> ||
    std::is_integral_v<T>;

template<typename T>
concept GPVRegVariant = std::is_same_v<T, aasm::GPReg> ||
    std::is_same_v<T, aasm::Address>;

template<typename T>
concept XOpVariant = std::is_same_v<T, aasm::XmmReg> ||
    std::is_same_v<T, aasm::Address> ||
    std::is_integral_v<T>;