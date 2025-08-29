#pragma once

#include "FieldAccess.h"

class GetElementPtr final: public FieldAccess {
public:
    explicit GetElementPtr(const NonTrivialType* basic_type, const Value &pointer, const Value &index) noexcept:
        FieldAccess(basic_type, pointer, index) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<GetElementPtr> gep(const NonTrivialType* basic_type, const Value &pointer, const Value &index) {
        return std::make_unique<GetElementPtr>(basic_type, pointer, index);
    }
};