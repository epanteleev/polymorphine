#pragma once
#include "NonTrivialType.h"


class AggregateType: public NonTrivialType {
public:
    [[nodiscard]]
    virtual const NonTrivialType* type_by_index(std::size_t index) const = 0;
};
