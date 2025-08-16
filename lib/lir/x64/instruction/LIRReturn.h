#pragma once

#include "lir/x64/instruction/LIRControlInstruction.h"


class LIRReturn final: public LIRControlInstruction {
public:
    explicit LIRReturn(std::vector<LIROperand>&& values) :
        LIRControlInstruction(std::move(values), {}) {}

    void visit(LIRVisitor &visitor) override {
        const auto ret_values = to_lir_vals_only(inputs());
        visitor.ret(ret_values);
    }

    static std::unique_ptr<LIRReturn> ret(const LIROperand& value) {
        return std::make_unique<LIRReturn>(std::vector{value});
    }

    static std::unique_ptr<LIRReturn> ret() {
        return std::make_unique<LIRReturn>(std::vector<LIROperand>{});
    }
};
