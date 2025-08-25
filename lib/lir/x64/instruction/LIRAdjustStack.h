#pragma once

#include "LIRInstructionBase.h"
#include "asm/x64/reg/RegSet.h"

enum class LIRAdjustKind: std::uint8_t {
    UpStack,
    DownStack,
    Prologue,
    Epilogue,
};

class LIRAdjustStack final: public LIRInstructionBase {
public:
    explicit LIRAdjustStack(LIRAdjustKind adjust_kind) noexcept:
        LIRInstructionBase(std::vector<LIROperand>{}),
        m_adjust_kind(adjust_kind) {}

    ~LIRAdjustStack() override = default;

    void visit(LIRVisitor &visitor) override;

    void add_reg(const aasm::GPReg reg) noexcept {
        m_caller_saved_regs.emplace(reg);
    }

    void increase_stack_size(const std::size_t size) {
        m_overflow_argument_area_size += size;
    }

    [[nodiscard]]
    LIRAdjustKind adjust_kind() const noexcept {
        return m_adjust_kind;
    }

    static std::unique_ptr<LIRAdjustStack> up_stack() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::UpStack);
    }

    static std::unique_ptr<LIRAdjustStack> down_stack() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::DownStack);
    }

    static std::unique_ptr<LIRAdjustStack> prologue() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::Prologue);
    }

    static std::unique_ptr<LIRAdjustStack> epilogue() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::Epilogue);
    }

private:
    std::size_t m_overflow_argument_area_size{};
    aasm::GPRegSet m_caller_saved_regs{};
    LIRAdjustKind m_adjust_kind;
};
