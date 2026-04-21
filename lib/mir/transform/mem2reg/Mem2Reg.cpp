#include "Mem2Reg.h"

#include "mir/analysis/Analysis.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Store.h"
#include "mir/instruction/Unary.h"

#include <ranges>
#include <unordered_set>
#include <vector>

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

            rename();
            cleanup();
        }

    private:
        struct PhiSlot {
            Phi* phi;
            const Alloc* alloc;
            std::size_t slot;
        };

        void collect_promotable_allocs() {
            const auto& escape_analysis = m_manager.analyze<EscapeAnalysis>(&m_fn);

            std::vector<const Alloc*> candidates;
            for (const auto& bb: m_fn.basic_blocks()) {
                for (const auto& inst : bb.instructions()) {
                    const auto a = Alloc::cast(&inst);
                    if (a == nullptr) {
                        continue;
                    }
                    if (escape_analysis.escape_state(a) != EscapeState::NOESCAPE) {
                        continue;
                    }
                    if (PrimitiveType::cast(a->allocated_type()) == nullptr) {
                        continue;
                    }

                    candidates.push_back(a);
                }
            }

            // Reserve up-front so references into m_reaching_def are stable
            // for the remainder of the pass.
            m_promotable_allocs.reserve(candidates.size());
            m_reaching_def.reserve(candidates.size());
            for (const auto a : candidates) {
                m_promotable_allocs.insert(a);
                auto& stack = m_reaching_def[a];
                stack.reserve(8);
                stack.push_back(Value::undefined());
            }
        }

        [[nodiscard]]
        const Alloc* get_promotable_alloc(const Value& val) const {
            if (!val.is<ValueInstruction*>()) return nullptr;
            const auto a = Alloc::cast(val.get<ValueInstruction*>());
            if (a == nullptr) {
                return nullptr;
            }

            if (!m_promotable_allocs.contains(a)) {
                return nullptr;
            }

            return a;
        }

        // Push a new reaching def for `a`, logging the push for later rollback.
        void push_def(const Alloc* a, const Value& v) {
            m_reaching_def.find(a)->second.push_back(v);
            m_push_log.push_back(a);
        }

        // Top of the reaching-def stack for `a`. Guaranteed non-empty because
        // we seed every promotable alloc with `undefined()` up front.
        [[nodiscard]]
        const Value& top_def(const Alloc* a) const {
            return m_reaching_def.find(a)->second.back();
        }

        void process_block(BasicBlock* block) {
            for (auto& inst : block->instructions()) {
                // --- Inserted phi: it defines a new reaching value for its alloc ---
                if (auto phi = Phi::cast(&inst)) {
                    if (const auto it = m_inserted_phis.find(phi); it != m_inserted_phis.end()) {
                        push_def(it->second, Value{phi});
                        continue;
                    }
                }

                // --- Store to a promotable alloc: push stored value, mark dead ---
                if (auto store = Store::cast(&inst)) {
                    const auto a = get_promotable_alloc(store->pointer());
                    if (a != nullptr) {
                        push_def(a, store->value());
                        m_dead_instructions.emplace_back(block, store);
                        continue;
                    }
                }

                // --- Load from a promotable alloc: RAUW with reaching def, mark dead ---
                if (auto load = Unary::cast(&inst)) {
                    if (load->op() == UnaryOp::Load) {
                        const auto a = get_promotable_alloc(load->operand());
                        if (a != nullptr) {
                            load->replace_all_uses(top_def(a));
                            m_dead_instructions.emplace_back(block, load);
                            continue;
                        }
                    }
                }
            }

            // Fill phi operands in successors that expect a value from `block`.
            // Using a precomputed (predecessor -> phi slots) map turns this
            // O(phi_slots_for_block) instead of O(succ * phis * preds).
            if (const auto it = m_phi_slots_by_pred.find(block); it != m_phi_slots_by_pred.end()) {
                for (const auto& slot : it->second) {
                    slot.phi->update_operand(slot.slot, top_def(slot.alloc));
                }
            }
        }

        void rename() {
            // Iterative DFS over the dominator tree. Each block gets one
            // `enter` frame (do work + schedule children) and one `exit`
            // frame (pop the reaching defs it pushed).
            struct Frame {
                BasicBlock* block;
                std::size_t log_base;
                bool is_exit;
            };

            const auto& tree = m_manager.analyze<DominatorTreeEval>(&m_fn);

            std::vector<Frame> stack;
            stack.emplace_back(m_fn.first(), 0, /*is_exit=*/false);

            while (!stack.empty()) {
                const Frame frame = stack.back();
                stack.pop_back();

                if (frame.is_exit) {
                    while (m_push_log.size() > frame.log_base) {
                        const auto a = m_push_log.back();
                        m_push_log.pop_back();
                        m_reaching_def.find(a)->second.pop_back();
                    }
                    continue;
                }

                const auto log_base = m_push_log.size();
                process_block(frame.block);
                stack.emplace_back(frame.block, log_base, /*is_exit=*/true);

                // Children are independent subtrees; push in reverse so the
                // natural order is preserved when popping.
                for (auto child : std::views::reverse(tree.children(frame.block))) {
                    stack.emplace_back(child, 0, /*is_exit=*/false);
                }
            }
        }

        void cleanup() {
            for (const auto& [block, inst_id] : m_dead_instructions) {
                block->remove_instruction(inst_id);
            }

            for (const auto a : m_promotable_allocs) {
                const auto owner = a->owner();
                owner->remove_instruction(a);
            }
        }

        void insert_phis() {
            const auto& join_point_set = m_manager.analyze<JoinPointSet>(&m_fn);
            for (const auto& [bb, v_set]: join_point_set) {
                const auto pred_span = bb->predecessors();
                if (pred_span.empty()) {
                    continue;
                }

                std::vector<const BasicBlock*> blocks_template(pred_span.begin(), pred_span.end());

                for (const auto& v: v_set) {
                    const auto primitive_ty = PrimitiveType::cast(v->allocated_type());
                    assertion(primitive_ty != nullptr, "must be primitive type");

                    auto blocks = blocks_template;
                    const auto phi = bb->prepend(Phi::undef(primitive_ty, std::move(blocks)));
                    m_inserted_phis.emplace(phi, v);

                    // Index each (predecessor -> phi slot) so `rename` can
                    // update a successor's phi operand in O(1).
                    for (std::size_t i{}; i < pred_span.size(); ++i) {
                        m_phi_slots_by_pred[pred_span[i]].emplace_back(phi, v, i);
                    }
                }
            }
        }

        FunctionData& m_fn;
        AnalysisPassManager& m_manager;
        std::unordered_map<const Phi*, const Alloc*> m_inserted_phis;
        std::unordered_map<BasicBlock*, std::vector<PhiSlot>> m_phi_slots_by_pred;

        std::unordered_set<const Alloc*> m_promotable_allocs;
        std::unordered_map<const Alloc*, std::vector<Value>> m_reaching_def;
        std::vector<std::pair<BasicBlock*, const Instruction*>> m_dead_instructions;

        // Single log of "pushes" shared across the whole dom-tree traversal.
        // Each entry records which alloc's reaching-def stack was grown;
        // on exit from a dom-tree node we rewind to its recorded base.
        std::vector<const Alloc*> m_push_log;
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
                const auto phi = m_worklist.back();
                m_worklist.pop_back();

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

                // Skip phis we've already promoted to useful to avoid
                // re-walking their operands from the worklist.
                if (is_useful(phi)) {
                    continue;
                }

                mark_useful(phi);
                m_worklist.push_back(phi);
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
                m_worklist.push_back(phi);
            }
        }

        void prune_useless_phis() const {
            // Iterate `m_order` rather than `m_useful` so removal order is
            // deterministic across runs.
            for (auto& phi : m_order) {
                if (is_useful(phi)) {
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
            if (m_useful.emplace(phi, false).second) {
                m_order.push_back(phi);
            }
        }

        [[nodiscard]]
        bool is_useful(Phi* const phi) const noexcept {
            return m_useful.at(phi);
        }

        FunctionData& m_fn;
        AnalysisPassManager& m_manager;

        std::unordered_map<Phi*, bool> m_useful;
        std::vector<Phi*> m_order;
        std::vector<Phi*> m_worklist;
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
