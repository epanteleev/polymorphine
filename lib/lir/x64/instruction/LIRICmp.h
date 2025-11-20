#pragma once

#include <memory>

#include "LIRInstructionBase.h"

class LIRICmp final: public LIRInstructionBase {
public:
    explicit LIRICmp(std::vector<LIROperand>&& uses) noexcept:
        LIRInstructionBase(std::move(uses)) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRICmp> cmp(const LIROperand &lhs, const LIROperand &rhs) {
        return std::make_unique<LIRICmp>(std::vector{lhs, rhs});
    }
};
