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
    explicit LIRInstruction(const LIRInstKind kind, const LIRValType val_type, std::vector<LIROperand>&& uses) noexcept:
        LIRInstructionBase(std::move(uses)),
        m_kind(kind),
        m_val_type(val_type) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRInstruction> mov(const LIRValType val_type, const LIROperand& dst, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Mov, val_type, std::vector{dst, src});
    }

    static std::unique_ptr<LIRInstruction> mov_by_idx(const LIRValType val_type, const LIRVal& dst, const LIROperand& index, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::MovByIdx, val_type, std::vector<LIROperand>{dst, index, src});
    }

    static std::unique_ptr<LIRInstruction> store(const LIRValType val_type, const LIRVal& dst, const LIROperand& src) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Store, val_type, std::vector<LIROperand>{dst, src});
    }

    static std::unique_ptr<LIRInstruction> cmp(const LIRValType val_type, const LIROperand &lhs, const LIROperand &rhs) {
        return std::make_unique<LIRInstruction>(LIRInstKind::Cmp, val_type, std::vector{lhs, rhs});
    }

    static std::unique_ptr<LIRInstruction> store_by_offset(const LIRValType val_type, const LIROperand& pointer, const LIROperand& index, const LIROperand& value) {
        return std::make_unique<LIRInstruction>(LIRInstKind::StoreByOffset, val_type, std::vector{pointer, index, value});
    }

private:
    LIRInstKind m_kind;
    LIRValType m_val_type;
};
