#include "Mem2Reg.h"

#include "mir/analysis/Analysis.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Store.h"
#include "mir/instruction/Unary.h"
#include "mir/value/UsedValue.h"

#include <stack>
#include <unordered_set>

namespace {
    class RewritePrimitives final {
    public:
        explicit RewritePrimitives(FunctionData& fn, AnalysisPassManager& manager) noexcept:
            m_fn(fn),
            m_manager(manager) {}

        void run() {
            insert_phis();
            collect_promotable_allocs();
            if (m_promotable_allocs.empty()) {
                return;
            }

            build_dom_children();
            rename(m_fn.first());
            cleanup();
        }

    private:
        void collect_promotable_allocs() {
            const auto& escape_analysis = m_manager.analyze<EscapeAnalysis>(&m_fn);
            for (const auto& bb: m_fn.basic_blocks()) {
                for (const auto& inst : bb.instructions()) {
                    const auto* a = Alloc::cast(&inst);
                    if (a == nullptr) {
                        continue;
                    }
                    if (escape_analysis.escape_state(a) != EscapeState::NOESCAPE) {
                        continue;
                    }

                    if (PrimitiveType::cast(a->allocated_type()) == nullptr) {
                        continue;
                    }

                    m_promotable_allocs.insert(a);
                    m_reaching_def[a].push(Value::undefined());
                }
            }
        }

        void build_dom_children() {
            const auto& tree = m_manager.analyze<DominatorTreeEval>(&m_fn);
            for (const auto [block, idom]: tree.immediate_dominators()) {
                if (idom == nullptr) {
                    continue;
                }

                m_dom_children[idom].push_back(block);
            }
        }

        [[nodiscard]]
        const Alloc* get_promotable_alloc(const Value& val) const {
            if (!val.is<ValueInstruction*>()) return nullptr;
            const auto* a = Alloc::cast(val.get<ValueInstruction*>());
            if (a == nullptr) {
                return nullptr;
            }

            if (!m_promotable_allocs.contains(a)) {
                return nullptr;
            }

            return a;
        }

        static void replace_all_uses(ValueInstruction* old_val, const Value& new_val) {
            for (auto* user : old_val->users()) {
                const auto idx = index_of(user->operands(), old_val);
                user->update_operand(idx, new_val);
            }
        }

        void rename(BasicBlock* block) {
            // Track how many defs we push per alloc so we can pop them later.
            std::unordered_map<const Alloc*, std::size_t> num_pushed;

            for (auto& inst : block->instructions()) {
                // --- Inserted phi: it defines a new reaching value for its alloc ---
                if (auto* phi = Phi::cast(&inst)) {
                    if (const auto it = m_inserted_phis.find(phi); it != m_inserted_phis.end()) {
                        const auto* a = it->second;
                        m_reaching_def[a].emplace(phi);
                        num_pushed[a]++;
                        continue;
                    }
                }

                // --- Store to a promotable alloc: push stored value, mark dead ---
                if (auto* store = Store::cast(&inst)) {
                    const auto* a = get_promotable_alloc(store->pointer());
                    if (a != nullptr) {
                        m_reaching_def[a].push(store->value());
                        num_pushed[a]++;
                        m_dead_instructions.emplace_back(block, store);
                        continue;
                    }
                }

                // --- Load from a promotable alloc: RAUW with reaching def, mark dead ---
                if (auto* load = Unary::cast(&inst)) {
                    if (load->op() == UnaryOp::Load) {
                        const auto* a = get_promotable_alloc(load->operand());
                        if (a != nullptr) {
                            replace_all_uses(load, m_reaching_def[a].top());
                            m_dead_instructions.emplace_back(block, load);
                            continue;
                        }
                    }
                }
            }

            // Fill in phi operands in each CFG successor.
            for (auto* succ : block->successors()) {
                for (auto& inst : succ->instructions()) {
                    auto* phi = Phi::cast(&inst);
                    if (phi == nullptr) break; // phis are always at the front

                    const auto it = m_inserted_phis.find(phi);
                    if (it == m_inserted_phis.end()) {
                        continue;
                    }

                    const auto* a = it->second;
                    const auto incoming = phi->incoming();
                    for (std::size_t i{}; i < incoming.size(); i++) {
                        if (incoming[i] == block) {
                            phi->update_operand(i, m_reaching_def[a].top());
                            break;
                        }
                    }
                }
            }

            // Recurse into dominator-tree children.
            if (const auto it = m_dom_children.find(block); it != m_dom_children.end()) {
                for (auto* child : it->second) {
                    rename(child);
                }
            }

            // Pop the definitions we pushed so siblings see the correct state.
            for (const auto& [a, count] : num_pushed) {
                for (std::size_t i{}; i < count; i++) {
                    m_reaching_def[a].pop();
                }
            }
        }

        void cleanup() {
            for (const auto& [block, inst_id] : m_dead_instructions) {
                block->remove_instruction(inst_id);
            }

            for (const auto* a : m_promotable_allocs) {
                const auto owner = a->owner();
                owner->remove_instruction(a);
            }
        }

        void insert_phis() {
            const auto& join_point_set = m_manager.analyze<JoinPointSet>(&m_fn);
            for (const auto& [bb, v_set]: join_point_set) {
                auto pred = bb->predecessors();
                std::vector<const BasicBlock*> blocks;
                blocks.reserve(pred.size());
                for (const auto& p: pred) {
                    blocks.push_back(p);
                }

                for (const auto& v: v_set) {
                    const auto primitive_ty = PrimitiveType::cast(v->allocated_type());
                    assertion(primitive_ty != nullptr, "must be primitive type");

                    auto copy = blocks;
                    const auto phi = bb->prepend(Phi::undef(primitive_ty, std::move(copy)));
                    m_inserted_phis.emplace(phi, v);
                }
            }
        }

        FunctionData& m_fn;
        AnalysisPassManager& m_manager;
        std::unordered_map<const Phi*, const Alloc*> m_inserted_phis;

        std::unordered_set<const Alloc*> m_promotable_allocs;
        std::unordered_map<const Alloc*, std::stack<Value>> m_reaching_def;
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> m_dom_children;
        std::vector<std::pair<BasicBlock*, const Instruction*>> m_dead_instructions;
    };

    class PhiFunctionPruning final {
    public:
        explicit PhiFunctionPruning(FunctionData& fn, AnalysisPassManager& manager) noexcept:
            m_fn(fn),
            m_manager(manager) {}

        void run() {
            initial_setup();
            propagate_usefulness();
            prune_useless_phis();
        }

    private:
        void initial_setup() {
            const auto& preorder = m_manager.analyze<PreorderTraverse>(&m_fn);
            for (const auto& block : preorder) {
                for (auto& inst : block->instructions()) {
                    if (const auto phi = Phi::cast(&inst); phi != nullptr) {
                        mark_useless(phi);
                        continue;
                    }

                    handle_operands(inst);
                }
            }
        }

        void propagate_usefulness() {
            while (!m_worklist.empty()) {
                const auto phi = m_worklist.top();
                m_worklist.pop();

                handle_phi_operands(phi);
            }
        }

        void handle_operands(const Instruction& inst) {
            for (const auto& op: inst.operands()) {
                if (!op.is<ValueInstruction*>()) {
                    continue;
                }

                const auto phi = Phi::cast(op.get<ValueInstruction*>());
                if (phi == nullptr) {
                    continue;
                }

                mark_useful(phi);
                m_worklist.push(phi);
            }
        }

        void handle_phi_operands(const Phi* inst) {
            for (const auto& op: inst->operands()) {
                if (!op.is<ValueInstruction*>()) {
                    continue;
                }

                const auto phi = Phi::cast(op.get<ValueInstruction*>());
                if (phi == nullptr) {
                    continue;
                }

                if (is_useful(phi)) {
                    continue;
                }

                mark_useful(phi);
                m_worklist.push(phi);
            }
        }

        void prune_useless_phis() {
            for (const auto &[phi, useful]: m_useful) {
                if (useful) {
                    continue;
                }
                phi->release();
            }

            for (const auto &[phi, useful]: m_useful) {
                if (useful) {
                    continue;
                }

                const auto owner = phi->owner();
                owner->remove_instruction_fast(phi);
            }
        }

        void mark_useful(Phi* const phi) noexcept {
            m_useful[phi] = true;
        }

        void mark_useless(Phi* const phi) noexcept {
            m_useful.emplace(phi, false);
        }

        [[nodiscard]]
        bool is_useful(Phi* const phi) const noexcept {
            return m_useful.at(phi);
        }

        FunctionData& m_fn;
        AnalysisPassManager& m_manager;

        std::unordered_map<Phi*, bool> m_useful;
        std::stack<Phi*> m_worklist;
    };
}

void Mem2Reg::run() noexcept {
    RewritePrimitives rewrite(m_fn, m_manager);
    rewrite.run();

    PhiFunctionPruning prune(m_fn, m_manager);
    prune.run();
}

Mem2Reg Mem2Reg::create(FunctionData &fn) noexcept {
    return Mem2Reg(fn);
}