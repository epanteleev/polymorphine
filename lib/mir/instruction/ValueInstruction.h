#pragma once

#include "mir/instruction/Instruction.h"
#include "mir/types/PointerType.h"
#include "mir/value/Use.h"

class ValueInstruction : public Instruction, public Use {
public:
    explicit ValueInstruction(const Type* ty, std::vector<Value>&& values) noexcept:
        Instruction(std::move(values)), Use(ty) {}

    void replace_all_uses(const Value& new_val) noexcept;

    [[nodiscard]]
    static const ValueInstruction* cast(const Instruction* inst) noexcept {
        return dynamic_cast<const ValueInstruction*>(inst);
    }

    [[nodiscard]]
    static ValueInstruction* cast(Instruction* inst) noexcept {
        return dynamic_cast<ValueInstruction*>(inst);
    }
};

namespace impls {
    inline bool ptr_type_inst(const Value& value) noexcept {
        if (!value.is<ValueInstruction*>()) {
            return false;
        }

        const auto value_instruction = value.get<ValueInstruction*>();
        return PointerType::cast(value_instruction->type()) != nullptr;
    }
}

consteval auto ptr_type_inst() noexcept {
    return impls::ptr_type_inst;
}