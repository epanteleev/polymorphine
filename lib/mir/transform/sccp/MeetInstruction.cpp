#include "MeetInstruction.h"

#include <ranges>

#include "mir/instruction/Icmp.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/InstructionVisitor.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Select.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "mir/instruction/Unary.h"
#include "mir/value/VConstant.h"

namespace details {
    bool MeetInstruction::meet(const Instruction &inst) noexcept {
        inst.visit(*this);
        return m_changed;
    }

    [[nodiscard]]
    Value make_int_value(const IntegerType* type, const std::int64_t value) noexcept {
        if (SignedIntegerType::cast(type) != nullptr) {
            switch (type->size_of()) {
                case 1: return Value::i8(static_cast<std::int8_t>(value));
                case 2: return Value::i16(static_cast<std::int16_t>(value));
                case 4: return Value::i32(static_cast<std::int32_t>(value));
                case 8: return Value::i64(value);
                default: std::unreachable();
            }
        }

        assertion(UnsignedIntegerType::cast(type) != nullptr, "expected unsigned integer type");
        switch (type->size_of()) {
            case 1: return Value::u8(static_cast<std::uint8_t>(value));
            case 2: return Value::u16(static_cast<std::uint16_t>(value));
            case 4: return Value::u32(static_cast<std::uint32_t>(value));
            case 8: return Value::u64(static_cast<std::uint64_t>(value));
            default: std::unreachable();
        }
    }

    [[nodiscard]]
    std::optional<bool> as_condition_value(const Value& value) noexcept {
        const auto fun = []<typename T>(const T& raw) -> std::optional<bool> {
            if constexpr (std::same_as<T, bool>) {
                return raw;
            } else {
                return std::nullopt;
            }
        };
        return value.visit(fun);
    }

    [[nodiscard]]
    Value make_fp_value(const FloatingPointType* type, const double value) noexcept {
        switch (type->size_of()) {
            case 4: return Value::f32(static_cast<float>(value));
            case 8: return Value::f64(value);
            default: std::unreachable();
        }
    }

    void MeetInstruction::terminator(const Instruction *inst) noexcept {
        const auto term = Terminator::from(inst);
        if (!term.has_value()) {
            return;
        }

        for (const auto* succ: term->targets()) {
            m_changed |= m_reachable_blocks.emplace(succ).second;
        }
    }

    void MeetInstruction::accept(Binary *inst) {
        const auto lhs = m_states.lattice_of_operand(inst->lhs());
        const auto rhs = m_states.lattice_of_operand(inst->rhs());
        if (lhs.kind() == LatticeKind::Overdefined || rhs.kind() == LatticeKind::Overdefined) {
            m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
            return;
        }
        if (lhs.kind() != LatticeKind::Constant || rhs.kind() != LatticeKind::Constant) {
            m_changed |= m_states.merge_state(inst, LatticeValue::unknown());
            return;
        }

        if (const auto* int_type = IntegerType::cast(inst->type())) {
            const auto lhs_i = lhs.value().get<std::int64_t>();
            const auto rhs_i = rhs.value().get<std::int64_t>();
            switch (inst->op()) {
                case BinaryOp::Add: {
                    const auto cst = make_int_value(int_type, lhs_i + rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::Subtract: {
                    const auto cst = make_int_value(int_type, lhs_i - rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::Multiply: {
                    const auto cst = make_int_value(int_type, lhs_i * rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::Divide: {
                    if (rhs_i == 0) {
                        m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
                        return;
                    }

                    const auto cst = make_int_value(int_type, lhs_i / rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::BitwiseAnd: {
                    const auto cst = make_int_value(int_type, lhs_i & rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::BitwiseOr: {
                    const auto cst = make_int_value(int_type, lhs_i | rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::BitwiseXor: {
                    const auto cst = make_int_value(int_type, lhs_i ^ rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::ShiftLeft: {
                    if (rhs_i < 0 || rhs_i >= 64) {
                        m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
                        return;
                    }

                    const auto cst = make_int_value(int_type, lhs_i << rhs_i);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                case BinaryOp::ShiftRight: {
                    if (rhs_i < 0 || rhs_i >= 64) {
                        m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
                        return;
                    }
                    if (SignedIntegerType::cast(int_type) != nullptr) {
                        const auto cst = make_int_value(int_type, lhs_i >> rhs_i);
                        m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                        return;
                    }
                    const auto lhs_u = static_cast<std::uint64_t>(lhs_i);
                    const auto res_u = lhs_u >> rhs_i;
                    const auto cst = make_int_value(int_type, static_cast<std::int64_t>(res_u));
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(cst));
                    return;
                }
                default: std::unreachable();
            }
        }

        if (const auto* fp_type = FloatingPointType::cast(inst->type())) {
            const auto lhs_fp = lhs.value().get<double>();
            const auto rhs_fp = rhs.value().get<double>();
            switch (inst->op()) {
                case BinaryOp::Add: {
                    const auto fp = make_fp_value(fp_type, lhs_fp + rhs_fp);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(fp));
                    return;
                }
                case BinaryOp::Subtract: {
                    const auto fp = make_fp_value(fp_type, lhs_fp - rhs_fp);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(fp));
                    return;
                }
                case BinaryOp::Multiply: {
                    const auto fp = make_fp_value(fp_type, lhs_fp * rhs_fp);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(fp));
                    return;
                }
                case BinaryOp::Divide: {
                    const auto fp = make_fp_value(fp_type, lhs_fp / rhs_fp);
                    m_changed |= m_states.merge_state(inst, LatticeValue::constant(fp));
                    return;
                }
                default: {
                    m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
                    return;
                }
            }
        }

        m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
    }

    void MeetInstruction::accept(Branch *branch) {
        terminator(branch);
    }

    void MeetInstruction::accept(CondBranch *cond_branch) {
        const auto cond_state = m_states.lattice_of_operand(cond_branch->condition());
        switch (cond_state.kind()) {
            case LatticeKind::Constant: {
                const auto cond_value = as_condition_value(cond_state.value());
                if (!cond_value.has_value()) {
                    return;
                }

                const auto* target = cond_value.value() ? cond_branch->on_true() : cond_branch->on_false();
                m_changed = m_reachable_blocks.emplace(target).second;
                return;
            }
            default: {
                const auto ins_true = m_reachable_blocks.emplace(cond_branch->on_true()).second;
                const auto ins_false = m_reachable_blocks.emplace(cond_branch->on_false()).second;
                m_changed = ins_true || ins_false;
            }
        }
    }

    void MeetInstruction::accept(Call *inst) {
        terminator(inst);
    }

    void MeetInstruction::accept(Return *inst) {
        terminator(inst);
    }

    void MeetInstruction::accept(ReturnValue *inst) {
        terminator(inst);
    }

    void MeetInstruction::accept(Switch *inst) {
        terminator(inst);
    }

    void MeetInstruction::accept(VCall *call) {
        terminator(call);
    }

    void MeetInstruction::accept(IVCall *call) {
        terminator(call);
    }

    void MeetInstruction::accept(Phi *inst) {
        std::optional<Value> acc;
        for (const auto& [incoming, op]: std::ranges::views::zip(inst->incoming(), inst->operands())) {
            if (!m_reachable_blocks.contains(incoming)) {
                continue;
            }

            const auto lattice = m_states.lattice_of_operand(op);
            if (lattice.kind() == LatticeKind::Overdefined) {
                m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
                return;
            }
            if (lattice.kind() == LatticeKind::Unknown) {
                m_changed |= m_states.merge_state(inst, LatticeValue::unknown());
                return;
            }

            if (!acc.has_value()) {
                acc = lattice.value();
                continue;
            }

            if (acc.value() != lattice.value()) {
                m_changed |= m_states.merge_state(inst, LatticeValue::overdefined());
                return;
            }
        }

        if (!acc.has_value()) {
            m_changed |= m_states.merge_state(inst, LatticeValue::unknown());
            return;
        }

        m_changed |= m_states.merge_state(inst, LatticeValue::constant(acc.value()));
    }

    void MeetInstruction::accept(IcmpInstruction *icmp) {
        const auto lhs = m_states.lattice_of_operand(icmp->lhs());
        const auto rhs = m_states.lattice_of_operand(icmp->rhs());
        if (lhs.kind() == LatticeKind::Overdefined || rhs.kind() == LatticeKind::Overdefined) {
            m_changed |= m_states.merge_state(icmp, LatticeValue::overdefined());
            return;
        }
        if (lhs.kind() != LatticeKind::Constant || rhs.kind() != LatticeKind::Constant) {
            m_changed |= m_states.merge_state(icmp, LatticeValue::unknown());
            return;
        }

        const auto lhs_i = lhs.value().get<std::int64_t>();
        const auto rhs_i = rhs.value().get<std::int64_t>();
        const bool is_signed = SignedIntegerType::cast(icmp->lhs().type()) != nullptr;

        bool pred_result = false;
        switch (icmp->predicate()) {
            case IcmpPredicate::Eq: pred_result = lhs_i == rhs_i; break;
            case IcmpPredicate::Ne: pred_result = lhs_i != rhs_i; break;
            case IcmpPredicate::Lt: {
                if (is_signed) {
                    pred_result = lhs_i < rhs_i;
                } else {
                    pred_result = static_cast<std::uint64_t>(lhs_i) < static_cast<std::uint64_t>(rhs_i);
                }
                break;
            }
            case IcmpPredicate::Le: {
                if (is_signed) {
                    pred_result = lhs_i <= rhs_i;
                } else {
                    pred_result = static_cast<std::uint64_t>(lhs_i) <= static_cast<std::uint64_t>(rhs_i);
                }
                break;
            }
            case IcmpPredicate::Gt: {
                if (is_signed) {
                    pred_result = lhs_i > rhs_i;
                } else {
                    pred_result = static_cast<std::uint64_t>(lhs_i) > static_cast<std::uint64_t>(rhs_i);
                }
                break;
            }
            case IcmpPredicate::Ge: {
                if (is_signed) {
                    pred_result = lhs_i >= rhs_i;
                } else {
                    pred_result = static_cast<std::uint64_t>(lhs_i) >= static_cast<std::uint64_t>(rhs_i);
                }
                break;
            }
            default: std::unreachable();
        }

        const auto boolean = pred_result ? Value::true_value() : Value::false_value();
        m_changed |= m_states.merge_state(icmp, LatticeValue::constant(boolean));
    }

    void MeetInstruction::accept(Select *select) {
        const auto cond = m_states.lattice_of_operand(select->condition());
        if (cond.kind() == LatticeKind::Unknown) {
            m_changed |= m_states.merge_state(select, LatticeValue::unknown());
            return;
        }

        if (cond.kind() == LatticeKind::Constant) {
            const auto cond_value = as_condition_value(cond.value());
            if (!cond_value.has_value()) {
                m_changed |= m_states.merge_state(select, LatticeValue::overdefined());
                return;
            }

            const auto branch = cond_value.value() ? select->on_true() : select->on_false();
            m_changed |= m_states.merge_state(select, m_states.lattice_of_operand(branch));
            return;
        }

        const auto on_true = m_states.lattice_of_operand(select->on_true());
        const auto on_false = m_states.lattice_of_operand(select->on_false());

        if (on_true.kind() == LatticeKind::Constant && on_false.kind() == LatticeKind::Constant) {
            if (on_true.value() == on_false.value()) {
                m_changed |= m_states.merge_state(select, on_true);
                return;
            }

            m_changed |= m_states.merge_state(select, LatticeValue::overdefined());
            return;
        }
        if (on_true.kind() == LatticeKind::Overdefined || on_false.kind() == LatticeKind::Overdefined) {
            m_changed |= m_states.merge_state(select, LatticeValue::overdefined());
            return;
        }

        m_changed |= m_states.merge_state(select, LatticeValue::unknown());
    }
}