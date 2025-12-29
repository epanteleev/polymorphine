#pragma once

#include "ArithmeticType.h"


class FloatingPointType final : public ArithmeticType {
    constexpr explicit FloatingPointType(const std::size_t size) noexcept:
        m_size(size) {}

public:
    [[nodiscard]]
    std::size_t size_of() const override {
        return m_size;
    }

    void visit(type::Visitor &visitor) override { visitor.accept(this); }

    static consteval const FloatingPointType *f32() noexcept {
        static constexpr FloatingPointType f32_instance(4);
        return &f32_instance;
    }

    static consteval const FloatingPointType *f64() noexcept {
        static constexpr FloatingPointType f64_instance(8);
        return &f64_instance;
    }

private:
    const std::size_t m_size;
};
