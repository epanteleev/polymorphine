#pragma once

#include "mir/instruction/Instruction.h"
#include "mir/value/Use.h"

class ValueInstruction : public Instruction, public Use {
public:
    explicit ValueInstruction(const Type* ty, std::vector<Value>&& values) noexcept:
        Instruction(std::move(values)), Use(ty) {}
};