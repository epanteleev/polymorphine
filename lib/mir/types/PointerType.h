#pragma once

#include "Type.h"

class PointerType final : public PrimitiveType {
public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return 8;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const PointerType *ptr() noexcept {
        static constexpr PointerType p;
        return &p;
    }
};