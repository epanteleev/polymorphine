#pragma once

#include "Type.h"

class FlagType final: public Type {
    FlagType() = default;
public:

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const FlagType *flag() noexcept {
        static constexpr FlagType flag_instance;
        return &flag_instance;
    }
};