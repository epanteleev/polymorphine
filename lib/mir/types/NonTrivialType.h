#pragma once

#include "Type.h"

class NonTrivialType : public Type {
public:
    [[nodiscard]]
    virtual std::size_t size_of() const = 0;

    [[nodiscard]]
    virtual std::size_t align_of() const = 0;
};
