#pragma once
#include "NonTrivialType.h"


class AggregateType: public NonTrivialType {
public:
    [[nodiscard]]
    virtual const NonTrivialType* field_type_of(std::size_t index) const = 0;

    [[nodiscard]]
    virtual std::size_t length() const noexcept = 0;
};
