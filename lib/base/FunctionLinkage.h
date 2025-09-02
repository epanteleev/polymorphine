#pragma once

#include <cstdint>
#include <iosfwd>


enum class FunctionLinkage: std::uint8_t {
    DEFAULT,
    EXTERN,
    INTERNAL
};

std::ostream & operator<<(std::ostream &os, const FunctionLinkage &linkage);