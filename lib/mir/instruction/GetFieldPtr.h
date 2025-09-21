#pragma once

#include "FieldAccess.h"
#include "mir/types/StructType.h"

class GetFieldPtr final: public FieldAccess {
public:
    explicit GetFieldPtr(const StructType* basic_type, const Value &pointer, const std::size_t index) noexcept:
        FieldAccess({pointer}),
        m_basic_type(basic_type),
        m_index(index) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    std::size_t index() const noexcept {
        return m_index;
    }

    [[nodiscard]]
    const StructType* basic_type() const noexcept {
        return m_basic_type;
    }

    [[nodiscard]]
    const NonTrivialType *access_type() const noexcept override {
        return m_basic_type->field_type_of(m_index);
    }

    static std::unique_ptr<GetFieldPtr> gfp(const StructType* basic_type, const Value &pointer, const std::size_t index) {
        return std::make_unique<GetFieldPtr>(basic_type, pointer, index);
    }

private:
    const StructType* m_basic_type;
    std::size_t m_index{};
};
