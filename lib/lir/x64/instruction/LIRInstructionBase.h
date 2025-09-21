#pragma once

#include <iosfwd>

#include "base/CommonInstruction.h"
#include "lir/x64/asm/TemporalRegs.h"
#include "lir/x64/operand/LIROperand.h"
#include "lir/x64/instruction/LIRVisitor.h"

class LIRInstructionBase : public CommonInstruction<LIRBlock> {
    static constexpr auto NO_ID = std::numeric_limits<std::size_t>::max();

public:
    explicit LIRInstructionBase(std::vector<LIROperand> &&uses) noexcept:
        m_inputs(std::move(uses)) {}

    virtual ~LIRInstructionBase() = default;

    [[nodiscard]]
    std::span<LIROperand const> inputs() const noexcept {
        return m_inputs;
    }

    [[nodiscard]]
    const LIROperand& in(const std::size_t idx) const {
        return m_inputs.at(idx);
    }

    void in(const std::size_t idx, const LIROperand& new_op) {
        assertion(idx < m_inputs.size(), "index out of bounds");

        const auto& op = m_inputs[idx];
        if (const auto lir_val = LIRVal::try_from(op); lir_val.has_value()) {
            lir_val->kill_user(this);
        }

        m_inputs[idx] = new_op;

        if (const auto lir_val = LIRVal::try_from(new_op); lir_val.has_value()) {
            lir_val->add_user(this);
        }
    }

    virtual void visit(LIRVisitor& visitor) = 0;

    void print(std::ostream &os) const;

    template<typename Fn>
    [[nodiscard]]
    bool isa(const Fn& matcher) const noexcept {
        return matcher(this);
    }

    [[nodiscard]]
    const TemporalRegs& temporal_regs() const noexcept {
        return m_temporal_regs;
    }

    void init_temporal_regs(const TemporalRegs& temporal_regs) noexcept {
        assertion(m_temporal_regs.empty(), "temporal registers already initialized");
        m_temporal_regs = temporal_regs;
    }

protected:
    friend class LIRBlock;

    [[nodiscard]]
    static std::vector<LIRVal> to_lir_vals_only(const std::span<LIROperand const> inputs) {
        std::vector<LIRVal> lir_vals;
        lir_vals.reserve(inputs.size());
        for (const auto& in: inputs) {
            const auto vreg = LIRVal::try_from(in);
            assertion(vreg.has_value(), "invariant");
            lir_vals.push_back(vreg.value());
        }
        return lir_vals;
    }

    std::vector<LIROperand> m_inputs;
    TemporalRegs m_temporal_regs;
};