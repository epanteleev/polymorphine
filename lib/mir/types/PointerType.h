#pragma once

#include "PrimitiveType.h"
#include "base/Constant.h"

class PointerType final : public PrimitiveType {
public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return cst::POINTER_SIZE;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const PointerType *ptr() noexcept {
        static constexpr PointerType pointer;
        return &pointer;
    }
};