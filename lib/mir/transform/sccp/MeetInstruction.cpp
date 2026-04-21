#include "MeetInstruction.h"

#include <ranges>

#include "mir/instruction/Icmp.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/InstructionVisitor.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Select.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "mir/value/VConstant.h"

namespace details {
    void MeetInstruction::terminator(const Instruction *inst) noexcept {
        const auto term = Terminator::from(inst);
        if (!term.has_value()) {
            return;
        }

        for (const auto* succ: term->targets()) {
            m_cfg_worklist.push_back(succ);
        }
    }

    template <typename Fn>
    [[nodiscard]]
    LatticeValue evaluate(const LatticeValue& lhs, const LatticeValue& rhs, Fn&& fn) noexcept {
        const auto res = fn(lhs.cst(), rhs.cst());
        if (!res.has_value()) {
            return LatticeValue::overdefined();
        }

        return LatticeValue::constant(res.value());
    };

    void MeetInstruction::accept(Binary *inst) {
        const auto lhs = m_states.lattice_of_operand(inst->lhs());
        if (lhs.kind() == LatticeKind::Overdefined) {
            update(inst, LatticeValue::overdefined());
            return;
        }
        const auto rhs = m_states.lattice_of_operand(inst->rhs());
        if (rhs.kind() == LatticeKind::Overdefined) {
            update(inst, LatticeValue::overdefined());
            return;
        }
        if (lhs.kind() != LatticeKind::Constant || rhs.kind() != LatticeKind::Constant) {
            update(inst, LatticeValue::unknown());
            return;
        }

        LatticeValue result;
        switch (inst->op()) {
            case BinaryOp::Add: {
                result = evaluate(lhs, rhs, VConstant::sum);
                break;
            }
            case BinaryOp::Subtract: {
                result = evaluate(lhs, rhs, VConstant::sub);
                break;
            }
            case BinaryOp::Multiply: {
                result = evaluate(lhs, rhs, VConstant::mul);
                break;
            }
            case BinaryOp::Divide: {
                result = evaluate(lhs, rhs, VConstant::div);
                break;
            }
            default: die("unimpl");
        }

        update(inst, result);
    }

    void MeetInstruction::accept(Branch *branch) {
        terminator(branch);
    }

    void MeetInstruction::accept(CondBranch *cond_branch) {
        const auto cond_state = m_states.lattice_of_operand(cond_branch->condition());
        if (cond_state.kind() == LatticeKind::Constant && cond_state.cst().is<bool>()) {
            const auto* target = cond_state.cst().get<bool>() ? cond_branch->on_true() : cond_branch->on_false();
            m_cfg_worklist.push_back(target);
            return;
        }

        m_cfg_worklist.push_back(cond_branch->on_true());
        m_cfg_worklist.push_back(cond_branch->on_false());
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
        std::optional<VConstant> acc;
        for (const auto& [incoming, op]: std::ranges::views::zip(inst->incoming(), inst->operands())) {
            if (!m_reachable_blocks.contains(incoming)) {
                continue;
            }

            switch (const auto lattice = m_states.lattice_of_operand(op); lattice.kind()) {
                case LatticeKind::Unknown: {
                    update(inst, LatticeValue::unknown());
                    return;
                }
                case LatticeKind::Constant: {
                    if (!acc.has_value()) {
                        acc = lattice.cst();
                        continue;
                    }

                    if (acc.value() == lattice.cst()) {
                        continue;
                    }
                    [[fallthrough]];
                }
                case LatticeKind::Overdefined: {
                    update(inst, LatticeValue::overdefined());
                    return;
                }
            }
        }

        if (!acc.has_value()) {
            update(inst, LatticeValue::unknown());
            return;
        }

        update(inst, LatticeValue::constant(acc.value()));
    }

    void MeetInstruction::accept(IcmpInstruction *icmp) {
        const auto lhs = m_states.lattice_of_operand(icmp->lhs());
        if (lhs.kind() == LatticeKind::Overdefined) {
            update(icmp, LatticeValue::overdefined());
            return;
        }
        const auto rhs = m_states.lattice_of_operand(icmp->rhs());
        if (rhs.kind() == LatticeKind::Overdefined) {
            update(icmp, LatticeValue::overdefined());
            return;
        }
        if (lhs.kind() != LatticeKind::Constant || rhs.kind() != LatticeKind::Constant) {
            update(icmp, LatticeValue::unknown());
            return;
        }

        LatticeValue result;
        switch (icmp->predicate()) {
            case IcmpPredicate::Eq: {
                result = evaluate(lhs, rhs, VConstant::eq);
                break;
            }
            case IcmpPredicate::Ne: {
                result = evaluate(lhs, rhs, VConstant::ne);
                break;
            }
            case IcmpPredicate::Lt: {
                result = evaluate(lhs, rhs, VConstant::lt);
                break;
            }
            case IcmpPredicate::Le: {
                result = evaluate(lhs, rhs, VConstant::le);
                break;
            }
            case IcmpPredicate::Gt: {
                result = evaluate(lhs, rhs, VConstant::gt);
                break;
            }
            case IcmpPredicate::Ge: {
                result = evaluate(lhs, rhs, VConstant::ge);
                break;
            }
            default: std::unreachable();
        }

        update(icmp, result);
    }

    void MeetInstruction::accept(Select *select) {
        const auto cond = m_states.lattice_of_operand(select->condition());
        if (cond.kind() == LatticeKind::Unknown) {
            update(select, LatticeValue::unknown());
            return;
        }

        if (cond.kind() == LatticeKind::Constant) {
            const auto& cond_value = cond.cst();
            if (!cond_value.is<bool>()) {
                update(select, LatticeValue::overdefined());
                return;
            }

            const auto branch = cond_value.get<bool>() ? select->on_true() : select->on_false();
            update(select, m_states.lattice_of_operand(branch));
            return;
        }

        const auto on_true = m_states.lattice_of_operand(select->on_true());
        const auto on_false = m_states.lattice_of_operand(select->on_false());

        if (on_true.kind() == LatticeKind::Constant && on_false.kind() == LatticeKind::Constant) {
            if (on_true.cst() == on_false.cst()) {
                update(select, on_true);
                return;
            }

            update(select, LatticeValue::overdefined());
            return;
        }
        if (on_true.kind() == LatticeKind::Overdefined || on_false.kind() == LatticeKind::Overdefined) {
            update(select, LatticeValue::overdefined());
            return;
        }

        update(select, LatticeValue::unknown());
    }
}
