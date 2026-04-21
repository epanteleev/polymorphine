#include "Sccp.h"

#include "MeetInstruction.h"
#include "LatticeValue.h"
#include "SccpLattice.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/ValueInstruction.h"

namespace details {
    class SccpEval final {
    public:
        explicit SccpEval(FunctionData& fn) noexcept:
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
            std::vector<const BasicBlock*> cfg_worklist;
            std::vector<const ValueInstruction*> ssa_worklist;
            MeetInstruction merger(m_reachable_blocks, m_lattice, cfg_worklist, ssa_worklist);

            cfg_worklist.push_back(m_fn.first());
            while (!cfg_worklist.empty() || !ssa_worklist.empty()) {
                if (!cfg_worklist.empty()) {
                    const auto* bb = cfg_worklist.back();
                    cfg_worklist.pop_back();

                    if (m_reachable_blocks.insert(bb).second) {
                        m_reachable_ordered.push_back(bb);
                        for (const auto& inst : bb->instructions()) {
                            merger.meet(inst);
                        }
                    } else {
                        for (const auto& inst : bb->instructions()) {
                            if (Phi::cast(&inst) == nullptr) {
                                break;
                            }
                            merger.meet(inst);
                        }
                    }
                    continue;
                }

                const auto def = ssa_worklist.back();
                ssa_worklist.pop_back();

                for (const auto* user : def->users()) {
                    if (!m_reachable_blocks.contains(user->owner())) {
                        continue;
                    }

                    merger.meet(*user);
                }
            }
        }

        void rewrite_constants() const {
            for (const auto& [inst, state]: m_lattice) {
                if (state.kind() != LatticeKind::Constant) {
                    continue;
                }

                const_cast<ValueInstruction*>(inst)->replace_all_uses(state.cst());
            }
        }

        struct FoldableBranch {
            const CondBranch* cond;
            const BasicBlock* target;
        };

        [[nodiscard]]
        std::vector<FoldableBranch> collect_branches() const {
            std::vector<FoldableBranch> foldable;
            for (const auto* bb : m_reachable_ordered) {
                const auto last = bb->last();
                const auto cond = last.get<CondBranch>();
                if (cond == nullptr) {
                    continue;
                }

                const auto cond_state = m_lattice.lattice_of_operand(cond->condition());
                if (cond_state.kind() != LatticeKind::Constant || !cond_state.cst().is<bool>()) {
                    continue;
                }

                const auto* target = cond_state.cst().get<bool>() ? cond->on_true() : cond->on_false();
                foldable.push_back({cond, target});
            }

            return foldable;
        }

        void simplify_branches() const {
            for (const auto [cond, target] : collect_branches()) {
                const auto term = Terminator::from(cond);
                assertion(term.has_value(), "must be terminator");

                auto* owner = cond->owner();
                for (const auto* succ : term->targets()) {
                    const_cast<BasicBlock*>(succ)->remove_predecessor(owner);
                }

                owner->remove_instruction(cond);
                owner->ins(Branch::br(const_cast<BasicBlock*>(target)));
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
            std::vector<const ValueInstruction*> worklist;

            for (const auto* bb : m_reachable_ordered) {
                for (const auto& inst : bb->instructions()) {
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
                const auto* inst = worklist.back();
                worklist.pop_back();

                auto* owner = inst->owner();
                if (!m_reachable_blocks.contains(owner)) {
                    continue;
                }

                if (!inst->users().empty()) {
                    continue;
                }

                // Snapshot operand defs before removal: `remove_instruction` will
                // destroy `inst` and unhook it from each def's user list, which is
                // exactly the signal we use below to queue transitive dead defs.
                std::vector<const ValueInstruction*> operand_defs;
                operand_defs.reserve(inst->operands().size());
                for (const auto& operand : inst->operands()) {
                    if (!operand.is<ValueInstruction*>()) {
                        continue;
                    }
                    operand_defs.push_back(operand.get<ValueInstruction*>());
                }

                owner->remove_instruction(inst);

                for (const auto* def : operand_defs) {
                    if (def->users().empty()) {
                        worklist.push_back(def);
                    }
                }
            }
        }

        FunctionData& m_fn;
        std::unordered_set<const BasicBlock*> m_reachable_blocks;
        std::vector<const BasicBlock*> m_reachable_ordered;
        SccpLattice m_lattice;
    };
}

void Sccp::run() noexcept {
    details::SccpEval eval(m_fn);
    eval.run();
}

Sccp Sccp::create(FunctionData &fn) noexcept {
    return Sccp(fn);
}
