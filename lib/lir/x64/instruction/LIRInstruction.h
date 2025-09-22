#pragma once

#include <memory>
#include "LIRInstructionBase.h"


enum class LIRInstKind: std::uint8_t {
    Mov,
    MovByIdx,
    StoreByOffset,
    Store,
    Cmp,
};

class LIRInstruction final: public LIRInstructionBase {
public:
    LIRInstruction(const LIRInstKind kind, std::vector<LIROperand>&& uses) :
        LIRInstructionBase(std::move(uses)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRInstruction> mov(const LIROperand& dst, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Mov, std::vector{dst, src});
    }

    static std::unique_ptr<LIRInstruction> mov_by_idx(const LIRVal& dst, const LIROperand& index, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::MovByIdx, std::vector<LIROperand>{dst, index, src});
    }

    static std::unique_ptr<LIRInstruction> store(const LIRVal& dst, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Store, std::vector<LIROperand>{dst, src});
    }

    static std::unique_ptr<LIRInstruction> cmp(const LIROperand &lhs, const LIROperand &rhs) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Cmp, std::vector{lhs, rhs});
    }

    static std::unique_ptr<LIRInstruction> store_by_offset(const LIROperand& pointer, const LIROperand& index, const LIROperand& value) {
        return std::make_unique<LIRInstruction>(LIRInstKind::StoreByOffset, std::vector{pointer, index, value});
    }

private:
    LIRInstKind m_kind;
};
