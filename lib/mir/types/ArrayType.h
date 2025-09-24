#pragma once

#include "AggregateType.h"
#include "utility/Error.h"


class ArrayType final: public AggregateType {
public:
    explicit ArrayType(const NonTrivialType* element_type, const std::size_t length) noexcept:
        m_element_type(element_type),
        m_length(length) {}

    void visit(type::Visitor &visitor) override {
        visitor.accept(this);
    }

    [[nodiscard]]
    std::size_t size_of() const override {
        return m_element_type->size_of() * m_length;
    }

    [[nodiscard]]
    std::size_t align_of() const override {
        return m_element_type->align_of();
    }

    [[nodiscard]]
    const NonTrivialType *field_type_of(const std::size_t index) const override {
        assertion(index < m_length, "Index out of bounds");
        return m_element_type;
    }

    [[nodiscard]]
    const NonTrivialType* element_type() const noexcept {
        return m_element_type;
    }

    [[nodiscard]]
    std::size_t length() const noexcept override {
        return m_length;
    }

    static ArrayType make(const NonTrivialType* element_type, const std::size_t length) {
        return ArrayType(element_type, length);
    }

private:
    const NonTrivialType* m_element_type;
    const std::size_t m_length;
};