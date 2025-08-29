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

class NonTrivialType : public Type {
public:
    [[nodiscard]]
    virtual std::size_t size_of() const = 0;

    [[nodiscard]]
    virtual std::size_t align_of() const = 0;
};

class PrimitiveType : public NonTrivialType {
public:
    [[nodiscard]]
    std::size_t align_of() const final {
        return size_of();
    }
};