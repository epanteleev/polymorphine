#include "Sccp.h"

#include <concepts>

#include "MeetInstruction.h"
#include "LatticeValue.h"
#include "SccpLattice.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/ValueInstruction.h"

namespace details {
    [[nodiscard]]
    static std::optional<bool> as_condition_value(const Value& value) noexcept {
        const auto fun = []<typename T>(const T& raw) -> std::optional<bool> {
            if constexpr (std::same_as<T, bool>) {
                return raw;
            } else {
                return std::nullopt;
            }
        };
        return value.visit(fun);
    }

    class SCCPEval final {
    public:
        explicit SCCPEval(FunctionData& fn) noexcept:
            m_fn(fn) {}

        void run() {
            process_function();
            rewrite_constants();
            simplify_branches();
            remove_dead_blocks();
            remove_dead_instructions();
        }

    private:
        void process_function() {
            m_reachable_blocks.emplace(m_fn.first());

            bool changed = false;
            do {
                changed = false;
                for (auto& bb: m_fn.basic_blocks()) {
                    if (!m_reachable_blocks.contains(&bb)) {
                        continue;
                    }

                    changed |= process_block(bb);
                }
            } while (changed);
        }

        bool process_block(const BasicBlock& bb) {
            bool changed = false;
            for (const auto& inst: bb.instructions()) {
                MeetInstruction merger(m_reachable_blocks, m_lattice);
                changed |= merger.meet(inst);
            }
            return changed;
        }

        void rewrite_constants() const {
            for (const auto& [inst, state]: m_lattice) {
                if (state.kind() != LatticeKind::Constant) {
                    continue;
                }

                for (const auto* user: inst->users()) {
                    const auto operands = user->operands();
                    for (std::size_t idx{}; idx < operands.size(); ++idx) {
                        const auto& op = operands[idx];
                        if (!op.is<ValueInstruction*>()) {
                            continue;
                        }
                        if (op.get<ValueInstruction*>() != inst) {
                            continue;
                        }

                        const_cast<Instruction*>(user)->update_operand(idx, state.value());
                    }
                }
            }
        }

        std::vector<const CondBranch*> collect_branches() const {
            std::vector<const CondBranch*> foldable;
            for (const auto& bb : m_fn.basic_blocks()) {
                if (!m_reachable_blocks.contains(&bb)) {
                    continue;
                }
                const auto last = bb.last();
                const auto cond = last.get<CondBranch>();
                if (cond == nullptr) {
                    continue;
                }

                const auto cond_state = m_lattice.lattice_of_operand(cond->condition());
                if (cond_state.kind() == LatticeKind::Constant) {
                    foldable.push_back(cond);
                }
            }

            return foldable;
        }

        void simplify_branches() const {
            for (const auto* cond : collect_branches()) {
                const auto cond_state = m_lattice.lattice_of_operand(cond->condition());
                const auto cond_value = as_condition_value(cond_state.value());

                assertion(cond_state.kind() == LatticeKind::Constant && cond_value.has_value(), "must be constant");

                auto* target = cond_value.value() ? cond->on_true() : cond->on_false();
                const auto term = Terminator::from(cond);
                assertion(term.has_value(), "must be terminator");

                auto* owner = cond->owner();
                for (const auto* succ : term->targets()) {
                    const_cast<BasicBlock*>(succ)->remove_predecessor(owner);
                }

                owner->remove_instruction(cond);
                owner->ins(Branch::br(target));
            }
        }

        void remove_dead_blocks() const {
            std::vector<const BasicBlock*> dead_blocks;
            for (const auto& bb : m_fn.basic_blocks()) {
                if (m_reachable_blocks.contains(&bb)) {
                    continue;
                }
                dead_blocks.push_back(&bb);
            }

            for (const auto* dead_block : dead_blocks) {
                const auto dead_term = dead_block->last();
                for (const auto* succ : dead_term.targets()) {
                    auto* succ_mut = const_cast<BasicBlock*>(succ);
                    for (auto& inst : succ_mut->instructions()) {
                        auto* phi = Phi::cast(&inst);
                        if (phi == nullptr) {
                            break;
                        }

                        phi->remove_incoming_from(dead_block);
                    }
                    succ_mut->remove_predecessor(dead_block);
                }

                m_fn.remove(dead_block);
            }
        }

        void remove_dead_instructions() const {
            std::deque<const ValueInstruction*> worklist;

            for (const auto& bb : m_fn.basic_blocks()) {
                if (!m_reachable_blocks.contains(&bb)) {
                    continue;
                }

                for (const auto& inst : bb.instructions()) {
                    const auto* value_inst = ValueInstruction::cast(&inst);
                    if (value_inst == nullptr) {
                        continue;
                    }
                    if (!value_inst->users().empty()) {
                        continue;
                    }

                    worklist.push_back(value_inst);
                }
            }

            while (!worklist.empty()) {
                const auto* inst = worklist.front();
                worklist.pop_front();

                auto* owner = inst->owner();
                if (!m_reachable_blocks.contains(owner)) {
                    continue;
                }

                if (!inst->users().empty()) {
                    continue;
                }

                for (const auto& operand : inst->operands()) {
                    if (!operand.is<ValueInstruction*>()) {
                        continue;
                    }

                    const auto* def = operand.get<ValueInstruction*>();
                    if (def->users().empty()) {
                        worklist.push_back(def);
                    }
                }

                owner->remove_instruction(inst);
            }
        }

        FunctionData& m_fn;
        std::unordered_set<const BasicBlock*> m_reachable_blocks;
        SccpLattice m_lattice;
    };
}

void Sccp::run() noexcept {
    details::SCCPEval eval(m_fn);
    eval.run();
}

Sccp Sccp::create(FunctionData &fn) noexcept {
    return Sccp(fn);
}
