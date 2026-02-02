#include "Mem2Reg.h"

#include "mir/analysis/Analysis.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Phi.h"

namespace {
    class RewritePrimitives final {
    public:
        explicit RewritePrimitives(FunctionData& fn, const DominatorTree<BasicBlock>& tree, const std::unordered_map<const Phi*, const Alloc*>& inserted_phis, const EscapeAnalysisResult& escape_analysis) noexcept:
            m_fn(fn),
            m_tree(tree),
            m_inserted_phis(inserted_phis),
            m_escape_analysis(escape_analysis) {}

        void run() {
            late_phi_rewrite();
        }

        void late_phi_rewrite() {
            std::unordered_set<const Phi*> phi_to_rewrite;
            //for (const auto& bb: )
        }

    private:
        FunctionData& m_fn;
        const DominatorTree<BasicBlock>& m_tree;
        const std::unordered_map<const Phi*, const Alloc*>& m_inserted_phis;
        const EscapeAnalysisResult& m_escape_analysis;
    };
}

void Mem2Reg::run() noexcept {
    [[maybe_unused]]
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