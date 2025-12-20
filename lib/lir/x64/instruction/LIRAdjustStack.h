#pragma once

#include <memory>

#include "LIRInstructionBase.h"
#include "asm/x64/reg/AnyRegSet.h"

enum class LIRAdjustKind: std::uint8_t {
    UpStack,
    DownStack,
    Prologue,
    Epilogue,
};

class LIRAdjustStack final: public LIRInstructionBase {
public:
    explicit LIRAdjustStack(const LIRAdjustKind adjust_kind) noexcept:
        LIRInstructionBase(std::vector<LIROperand>{}),
        m_adjust_kind(adjust_kind) {}

    ~LIRAdjustStack() override = default;

    void visit(LIRVisitor &visitor) override;

    void add_regs(const aasm::RegSet& regs) noexcept {
        assertion(m_caller_saved_regs.empty(), "invariant");
        m_caller_saved_regs = regs;
    }

    void increase_overflow_area_size(const std::size_t size) {
        m_overflow_argument_area_size += size;
    }

    void increase_local_area_size(const std::size_t size) {
        m_local_area_size += size;
    }

    [[nodiscard]]
    LIRAdjustKind adjust_kind() const noexcept {
        return m_adjust_kind;
    }

    [[nodiscard]]
    static std::unique_ptr<LIRAdjustStack> up_stack() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::UpStack);
    }

    [[nodiscard]]
    static std::unique_ptr<LIRAdjustStack> down_stack() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::DownStack);
    }

    [[nodiscard]]
    static std::unique_ptr<LIRAdjustStack> prologue() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::Prologue);
    }

    [[nodiscard]]
    static std::unique_ptr<LIRAdjustStack> epilogue() {
        return std::make_unique<LIRAdjustStack>(LIRAdjustKind::Epilogue);
    }

private:
    std::size_t m_overflow_argument_area_size{};
    std::size_t m_local_area_size{};
    aasm::RegSet m_caller_saved_regs{};
    LIRAdjustKind m_adjust_kind;
};
