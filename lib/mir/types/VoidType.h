#pragma once

#include "Type.h"

class VoidType final: public Type {
    constexpr VoidType() = default;

public:
    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const VoidType *type() noexcept {
        static constexpr VoidType instance;
        return &instance;
    }
};