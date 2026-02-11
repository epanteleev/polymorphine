#include "Mem2Reg.h"

#include "mir/analysis/Analysis.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Store.h"
#include "mir/instruction/Unary.h"

#include <stack>
#include <unordered_set>

namespace {
    class RewritePrimitives final {
    public:
        explicit RewritePrimitives(
            FunctionData& fn,
            const DominatorTree<BasicBlock>& tree,
            const std::unordered_map<const Phi*, const Alloc*>& inserted_phis,
            const EscapeAnalysisResult& escape_analysis
        ) noexcept:
            m_fn(fn),
            m_tree(tree),
            m_inserted_phis(inserted_phis),
            m_escape_analysis(escape_analysis) {}

        void run() {
            collect_promotable_allocs();
            if (m_promotable_allocs.empty()) return;
            build_dom_children();
            rename(m_fn.first());
            cleanup();
        }

    private:
        void collect_promotable_allocs() {
            for (const auto& bb : m_fn.basic_blocks()) {
                for (const auto& inst : bb.instructions()) {
                    const auto* a = Alloc::cast(&inst);
                    if (a == nullptr) continue;
                    if (m_escape_analysis.escape_state(a) != EscapeState::NOESCAPE) continue;
                    if (PrimitiveType::cast(a->allocated_type()) == nullptr) continue;

                    m_promotable_allocs.insert(a);
                    m_reaching_def[a].push(Value::undefined());
                }
            }
        }

        void build_dom_children() {
            for (const auto [block, idom] : m_tree.immediate_dominators()) {
                if (idom != nullptr) {
                    m_dom_children[idom].push_back(block);
                }
            }
        }

        [[nodiscard]]
        const Alloc* get_promotable_alloc(const Value& val) const {
            if (!val.is<ValueInstruction*>()) return nullptr;
            const auto* a = dynamic_cast<const Alloc*>(val.get<ValueInstruction*>());
            if (a == nullptr) return nullptr;
            if (!m_promotable_allocs.contains(a)) return nullptr;
            return a;
        }

        static void replace_all_uses(const ValueInstruction* old_val, const Value& new_val) {
            for (const auto* user : old_val->users()) {
                const_cast<Instruction*>(user)->replace_operand(old_val, new_val);
            }
        }

        void rename(BasicBlock* block) {
            // Track how many defs we push per alloc so we can pop them later.
            std::unordered_map<const Alloc*, std::size_t> num_pushed;

            for (auto& inst : block->instructions()) {
                // --- Inserted phi: it defines a new reaching value for its alloc ---
                if (auto* phi = dynamic_cast<Phi*>(&inst)) {
                    const auto it = m_inserted_phis.find(phi);
                    if (it != m_inserted_phis.end()) {
                        const auto* a = it->second;
                        m_reaching_def[a].push(Value(phi));
                        num_pushed[a]++;
                        continue;
                    }
                }

                // --- Store to a promotable alloc: push stored value, mark dead ---
                if (auto* store = dynamic_cast<Store*>(&inst)) {
                    const auto* a = get_promotable_alloc(store->pointer());
                    if (a != nullptr) {
                        m_reaching_def[a].push(store->value());
                        num_pushed[a]++;
                        m_dead_instructions.emplace_back(block, store);
                        continue;
                    }
                }

                // --- Load from a promotable alloc: RAUW with reaching def, mark dead ---
                if (auto* load = dynamic_cast<Unary*>(&inst)) {
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
                    auto* phi = dynamic_cast<Phi*>(&inst);
                    if (phi == nullptr) break; // phis are always at the front

                    const auto it = m_inserted_phis.find(phi);
                    if (it == m_inserted_phis.end()) continue;

                    const auto* a = it->second;
                    const auto incoming = phi->incoming();
                    for (std::size_t i = 0; i < incoming.size(); i++) {
                        if (incoming[i] == block) {
                            phi->set_incoming_value(i, m_reaching_def[a].top());
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
                a->owner()->remove_instruction(a);
            }
        }

        FunctionData& m_fn;
        const DominatorTree<BasicBlock>& m_tree;
        const std::unordered_map<const Phi*, const Alloc*>& m_inserted_phis;
        const EscapeAnalysisResult& m_escape_analysis;

        std::unordered_set<const Alloc*> m_promotable_allocs;
        std::unordered_map<const Alloc*, std::stack<Value>> m_reaching_def;
        std::unordered_map<BasicBlock*, std::vector<BasicBlock*>> m_dom_children;
        std::vector<std::pair<BasicBlock*, const Instruction*>> m_dead_instructions;
    };
}

void Mem2Reg::run() noexcept {
    const auto inserted_phis = insert_phis();
    RewritePrimitives rewrite(m_fn, m_tree, inserted_phis, m_escape_analysis);
    rewrite.run();
}

Mem2Reg Mem2Reg::create(FunctionData &fn) noexcept {
    AnalysisPassManager manager;
    const auto join_point_set = manager.analyze<JoinPointSet>(&fn);
    const auto dominator_tree = manager.analyze<DominatorTreeEval>(&fn);
    const auto escape_analysis = manager.analyze<EscapeAnalysis>(&fn);

    return Mem2Reg(fn, *dominator_tree, *join_point_set, *escape_analysis);
}

std::unordered_map<const Phi *, const Alloc *> Mem2Reg::insert_phis() const {
    std::unordered_map<const Phi*, const Alloc*> inserted_phis;

    for (const auto& [bb, v_set]: m_join_point_set) {
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
            inserted_phis.emplace(phi, v);
        }
    }
    return inserted_phis;
}
