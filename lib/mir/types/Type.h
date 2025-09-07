#pragma once

#include <cstddef>
#include <iosfwd>

#include "TypeVisitor.h"

class Type {
public:
    virtual ~Type() = default;

    template<typename Matcher>
    [[nodiscard]]
    bool isa(const Matcher &matcher) const noexcept {
        return matcher(this);
    }

    friend std::ostream& operator<<(std::ostream &os, const Type &obj);

    bool operator==(const Type &other) const {
        return this == &other;
    }

    bool operator!=(const Type &other) const {
        return !(*this == other);
    }

    virtual void visit(type::Visitor &visitor) = 0;
};

std::ostream& operator<<(std::ostream &os, const Type &obj);