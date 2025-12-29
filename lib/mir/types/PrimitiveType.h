#pragma once

#include "mir/types/NonTrivialType.h"

class PrimitiveType: public NonTrivialType {
public:
    [[nodiscard]]
    std::size_t align_of() const final { return size_of(); }
};