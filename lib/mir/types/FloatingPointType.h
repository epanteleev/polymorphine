#pragma once

#include "Type.h"

class FloatingPointType final : public PrimitiveType {
    explicit FloatingPointType(const std::size_t size) noexcept:
        m_size(size) {}

public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

private:
    const std::size_t m_size;
};
