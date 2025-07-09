#pragma once

#include "../mir_frwd.h"

using TypeMatcherItem = std::function<bool(const Type*)>;

struct TypeMatcher final {
    explicit TypeMatcher(TypeMatcherItem&& matcher) : ptr(std::move(matcher)) {}

    bool operator()(const Type* type) const {
        return ptr(type);
    }

    TypeMatcher operator ||(const TypeMatcher &other) const {
        return TypeMatcher([&](const Type *type) {
            return (*this)(type) || other(type);
        });
    }

    TypeMatcher operator &&(const TypeMatcher &other) const {
        return TypeMatcher([&](const Type *type) {
            return (*this)(type) && other(type);
        });
    }

private:
    TypeMatcherItem ptr;
};