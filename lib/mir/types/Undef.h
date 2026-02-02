#pragma once

#include "Type.h"

class Undef final: public Type {
    explicit Undef() = default;
public:
    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    static consteval const Undef *undef() noexcept {
        static constexpr Undef undef;
        return &undef;
    }

    [[nodiscard]]
    static constexpr const Undef* cast(const Type* ty) noexcept {
        if (ty == undef()) {
            return undef();
        }

        return nullptr;
    }
};