#pragma once

#include "FieldAccess.h"

class GetFieldPtr final: public FieldAccess {
public:
    explicit GetFieldPtr(const NonTrivialType* basic_type, const Value &pointer, const Value &index) noexcept:
        FieldAccess(basic_type, pointer, index) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<GetFieldPtr> gfp(const NonTrivialType* basic_type, const Value &pointer, const Value &index) {
        return std::make_unique<GetFieldPtr>(basic_type, pointer, index);
    }
};
